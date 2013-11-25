#include "surface_generationCage.h"

#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

MapParameters::MapParameters() :
    m_initialized(false),
    m_voxellisation(NULL),
    m_resolutions(NULL),
    m_bb(NULL),
    m_dilatation(0),
    m_toVoxellise(true)
{}

void MapParameters::start(const QString& mapName, const QString& positionAttributeName) {
    if(!m_initialized) {
        m_initialized = true;
        m_voxellisation = Algo::Surface::Modelisation::Voxellisation();
    }
}

void MapParameters::stop(const QString& mapName, const QString& positionAttributeName) {
    if(m_initialized) {
        m_initialized = false;
    }
}

bool Surface_GenerationCage_Plugin::enable()
{
    m_generationCageDialog = new Dialog_GenerationCage(m_schnapps);

    m_generationCageAction = new QAction("Generate cage", this);

    m_schnapps->addMenuAction(this, "Surface;Generate cage", m_generationCageAction);

    connect(m_generationCageAction, SIGNAL(triggered()), this, SLOT(openGenerationCageDialog()));

    connect(m_generationCageDialog->button_generate, SIGNAL(clicked()), this, SLOT(generationCageFromDialog()));
    connect(m_generationCageDialog->button_dilaterVoxellisation, SIGNAL(clicked()), this, SLOT(dilaterVoxellisationFromDialog()));

    connect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
    connect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));

    foreach(MapHandlerGen* map, m_schnapps->getMapSet().values())
        mapAdded(map);

    return true;
}

void Surface_GenerationCage_Plugin::disable()
{
    disconnect(m_generationCageAction, SIGNAL(triggered()), this, SLOT(openGenerationCageDialog()));

    disconnect(m_generationCageDialog->button_generate, SIGNAL(clicked()), this, SLOT(generationCageFromDialog()));
    disconnect(m_generationCageDialog->button_dilaterVoxellisation, SIGNAL(clicked()), this, SLOT(dilaterVoxellisationFromDialog()));

    disconnect(m_schnapps, SIGNAL(mapAdded(MapHandlerGen*)), this, SLOT(mapAdded(MapHandlerGen*)));
    disconnect(m_schnapps, SIGNAL(mapRemoved(MapHandlerGen*)), this, SLOT(mapRemoved(MapHandlerGen*)));
}

void Surface_GenerationCage_Plugin::mapAdded(MapHandlerGen *map)
{
    connect(map, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
}

void Surface_GenerationCage_Plugin::mapRemoved(MapHandlerGen *map)
{
    disconnect(map, SIGNAL(attributeModified(unsigned int, QString)), this, SLOT(attributeModified(unsigned int, QString)));
}

void Surface_GenerationCage_Plugin::attributeModified(unsigned int orbit, QString nameAttr)
{
    //Rien pour le moment
}

void Surface_GenerationCage_Plugin::openGenerationCageDialog()
{
    m_generationCageDialog->show();
}

void Surface_GenerationCage_Plugin::generationCageFromDialog()
{
    QList<QListWidgetItem*> currentItems = m_generationCageDialog->list_maps->selectedItems();
    if(!currentItems.empty())
    {
        const QString& mapName = currentItems[0]->text();

        QString positionName = m_generationCageDialog->combo_positionAttribute->currentText();

        generationCage(mapName, positionName);
    }
}

void Surface_GenerationCage_Plugin::dilaterVoxellisationFromDialog() {
    QList<QListWidgetItem*> currentItems = m_generationCageDialog->list_maps->selectedItems();
    if(!currentItems.empty())
    {
        const QString& mapName = currentItems[0]->text();

        QString positionName = m_generationCageDialog->combo_positionAttribute->currentText();

        dilaterVoxellisation(mapName, positionName);
    }
}

void Surface_GenerationCage_Plugin::generationCage(const QString& mapName, const QString& positionAttributeName) {
    MapParameters& p = h_parameterSet[mapName+positionAttributeName];

    p.start(mapName, positionAttributeName);

    if(p.m_toVoxellise) {
        MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_schnapps->getMap(mapName));
        PFP2::MAP* selectedMap = mh->getMap();
        VertexAttribute<PFP2::VEC3> position = selectedMap->getAttribute<PFP2::VEC3, VERTEX>(positionAttributeName.toStdString());
        if(!position.isValid()) {
            CGoGNout << "L'attribut de position choisi pour la carte sélectionnée n'est pas valide." << CGoGNendl;
            return;
        }
        p.m_bb = Algo::Geometry::computeBoundingBox<PFP2>(*selectedMap, position);
        calculateResolutions(mapName, positionAttributeName);
        voxellise(mapName, positionAttributeName);
        p.m_toVoxellise = false;
    }
    p.m_voxellisation.marqueVoxelsExterieurs();
    p.m_voxellisation.remplit();

    extractionCarte(mapName, positionAttributeName);
    m_generationCageDialog->updateResolutionsFromPlugin(p.m_resolutions);
}

void Surface_GenerationCage_Plugin::dilaterVoxellisation(const QString& mapName, const QString& positionAttributeName) {
    MapParameters& p = h_parameterSet[mapName+positionAttributeName];

    p.m_voxellisation.dilate();
    p.m_voxellisation.extractionBord();
    extractionCarte(mapName, positionAttributeName);
    m_generationCageDialog->updateNiveauDilatationFromPlugin(++p.m_dilatation);
}

void Surface_GenerationCage_Plugin::extractionCarte(const QString& mapName, const QString& positionAttributeName) {
    MapParameters& p = h_parameterSet[mapName+positionAttributeName];

    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_schnapps->getMap(mapName+QString("Cage")));

    if(mh==NULL) {
        //Si la carte représentant la cage de ce maillage n'existait pas encore
        mh = static_cast<MapHandler<PFP2>*>(m_schnapps->addMap(mapName+QString("Cage"), 2));
    }

    PFP2::MAP* mapCage = mh->getMap();
    mapCage->clear(true);

    VertexAttribute<PFP2::VEC3> positionCage = mapCage->getAttribute<PFP2::VEC3, VERTEX>("position");
    if(!positionCage.isValid()) {
        positionCage = mapCage->addAttribute<PFP2::VEC3, VERTEX>("position");
        mh->registerAttribute(positionCage);
    }

    if(m_generationCageDialog->radio_extractionFaces->isChecked()) {
        //Si l'algorithme choisi est celui de l'extraction de faces
        p.m_voxellisation.extractionBord();

        std::vector<std::string> attrNamesCage;
        //MERGE CLOSE VERTICES A 'TRUE' FAIT PLANTER L'APPLICATION
        if(!Algo::Surface::Import::importVoxellisation<PFP2>(*mapCage, p.m_voxellisation, attrNamesCage, false))
        {
            CGoGNerr << "Impossible d'importer la voxellisation" << CGoGNendl ;
            return;
        }
        positionCage = mapCage->getAttribute<PFP2::VEC3, VERTEX>(attrNamesCage[0]);
    }
    else if(m_generationCageDialog->radio_marchingCube->isChecked()){
        //Si l'algorithme choisi est celui du marching cube

        Algo::Surface::MC::Image<int>* image = p.m_voxellisation.getImage();   //On récupère l'image correspondant à la voxellisation
        Algo::Surface::MC::WindowingEqual<int> windowing;
        windowing.setIsoValue(2); //L'intérieur est représenté avec une valeur de '2'
        Algo::Surface::MC::MarchingCube<int, Algo::Surface::MC::WindowingEqual,PFP2> marching_cube(image, mapCage, positionCage, windowing, false);
        marching_cube.simpleMeshing();
        marching_cube.recalPoints(p.m_bb.min()-Geom::Vec3f(image->getVoxSizeX(), image->getVoxSizeY(), image->getVoxSizeZ()));
        delete image;
    }

    mh->notifyAttributeModification(positionCage);  //Met a jour le VBO

    mh->updateBB(positionCage); //Met a jour la boite englobante de la carte
}

void Surface_GenerationCage_Plugin::calculateResolutions(const QString& mapName, const QString& positionAttributeName) {
    MapParameters& p = h_parameterSet[mapName+positionAttributeName];

    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_schnapps->getMap(mapName));
    PFP2::MAP* selectedMap = mh->getMap();

    Geom::Vec3f bb_min = p.m_bb.min();
    Geom::Vec3f bb_max = p.m_bb.max();

    const float SPARSE_FACTOR=0.05f;

    //Résolution calculée avec la méthode de "Automatic Generation of Coarse Bounding Cages from Dense Meshes"
    int n = std::sqrt((selectedMap->getNbCells(VERTEX))*SPARSE_FACTOR/6);

    Algo::Surface::Modelisation::swapVectorMax(bb_min, bb_max);

    float delta_x = bb_max[0]-bb_min[0];
    float delta_y = bb_max[1]-bb_min[1];
    float delta_z = bb_max[2]-bb_min[2];

    float max = std::max(std::max(delta_x, delta_y), delta_z);  //On récupère la composante qui a l'écart maximum

    //On adapte la résolution calculée pour qu'elle soit différente dans chacune des composantes x, y et z
    do {
        //On recalcule les résolutions jusqu'à ce que chacune d'entre elle ne soit plus nulle
        p.m_resolutions[0] = n*delta_x/max;
        p.m_resolutions[1] = n*delta_y/max;
        p.m_resolutions[2] = n*delta_z/max;
        n+=5;
    } while(p.m_resolutions[0]==0 || p.m_resolutions[1]==0 || p.m_resolutions[2]==0);

    CGoGNout << "Initialisation des résolutions : Résolution en x = " << p.m_resolutions[0] << " | Résolution en y = "<< p.m_resolutions[1] << " | Résolution en z = " << p.m_resolutions[2] << CGoGNendl;

    m_generationCageDialog->updateResolutionsFromPlugin(p.m_resolutions);

    p.m_toVoxellise = true;
}

/*
  * Fonction qui met à jour les résolutions dans chacun des composantes en fonction de la valeur choisie en x
  */
void Surface_GenerationCage_Plugin::updateResolutions(const QString& mapName, const QString& positionAttributeName, bool independant) {
    MapParameters& p = h_parameterSet[mapName+positionAttributeName];

    if(!independant) {
        //Si les coordonnées sont calculées de façon indépendante
        Geom::Vec3f bb_min = p.m_bb.min();
        Geom::Vec3f bb_max = p.m_bb.max();

        int res_x = p.m_resolutions[0];

        Algo::Surface::Modelisation::swapVectorMax(bb_min, bb_max);

        float delta_x = bb_max[0]-bb_min[0];
        float delta_y = bb_max[1]-bb_min[1];
        float delta_z = bb_max[2]-bb_min[2];

        float max = std::max(std::max(delta_x, delta_y), delta_z);  //On récupère la composante qui a l'écart maximum

        if(res_x<=0)
            res_x = 1;

        int n = res_x*max/delta_x;

        //On adapte la résolution calculée pour qu'elle soit différente dans chacune des composantes x, y et z
        p.m_resolutions[0] = n*delta_x/max;
        p.m_resolutions[1] = n*delta_y/max;
        p.m_resolutions[2] = n*delta_z/max;
    }
    else {
        int res_x = m_generationCageDialog->spin_resolution_x->text().toInt();
        int res_y = m_generationCageDialog->spin_resolution_y->text().toInt();
        int res_z = m_generationCageDialog->spin_resolution_z->text().toInt();
        p.m_resolutions[0] = res_x>0?res_x:1;
        p.m_resolutions[1] = res_y>0?res_y:1;
        p.m_resolutions[2] = res_z>0?res_z:1;
    }

    CGoGNout << "Modification des résolutions : Résolution en x = " << p.m_resolutions[0] << " | Résolution en y = "<< p.m_resolutions[1] << " | Résolution en z = " << p.m_resolutions[2] << CGoGNendl;

    m_generationCageDialog->updateResolutionsFromPlugin(p.m_resolutions);

    p.m_toVoxellise = true;
}

void Surface_GenerationCage_Plugin::voxellise(const QString& mapName, const QString& positionAttributeName) {
    MapHandlerGen* mhg = m_schnapps->getMap(mapName);
    MapParameters& p = h_parameterSet[mapName+positionAttributeName];

    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(mhg);
    PFP2::MAP* selectedMap = mh->getMap();

    VertexAttribute<PFP2::VEC3> position = selectedMap->getAttribute<PFP2::VEC3, VERTEX>(positionAttributeName.toStdString());
    if(!position.isValid()) {
        CGoGNout << "L'attribut de position choisi pour la carte sélectionnée n'est pas valide." << CGoGNendl;
        return;
    }

    VertexAttribute<Voxel> voxel = selectedMap->getAttribute<Voxel, VERTEX>("voxel");
    if(!voxel.isValid()) {
        voxel = selectedMap->addAttribute<Voxel, VERTEX>("voxel");
    }

    p.m_voxellisation = Algo::Surface::Modelisation::Voxellisation(p.m_resolutions[0], p.m_resolutions[1], p.m_resolutions[2], p.m_bb);

    TraversorF<PFP2::MAP> trav_face_map(*selectedMap);
    std::vector<Geom::Vec3i> polygone = std::vector<Geom::Vec3i>();

    for(Dart d = trav_face_map.begin(); d!=trav_face_map.end(); d=trav_face_map.next()) {
        //On traverse l'ensemble des faces de la carte
        Traversor2FV<PFP2::MAP> trav_face_vert(*selectedMap,d);
        polygone.clear();
        for(Dart e = trav_face_vert.begin(); e!=trav_face_vert.end(); e=trav_face_vert.next()) {
            //On récupère les sommets composants la face courante
            voxel[e].setIndexes(getVoxelIndex(mapName, positionAttributeName, position[e]));
            polygone.push_back(voxel[e].getIndexes());
        }
        p.m_voxellisation.voxellisePolygone(polygone);
    }

    CGoGNout << "Fin de la voxellisation. Il y a " << p.m_voxellisation.size() << " voxel(s) qui entourent le maillage" << CGoGNendl;
}

/*
  * Fonction permettant de récupérer les indices du voxel associé au sommet 'a'
  */
Geom::Vec3i& Surface_GenerationCage_Plugin::getVoxelIndex(const QString& mapName, const QString& positionAttributeName, Geom::Vec3f a) {
    MapParameters& p = h_parameterSet[mapName+positionAttributeName];

    Geom::Vec3i* voxel = new Geom::Vec3i();

    Geom::Vec3f bb_min = p.m_bb.min();
    Geom::Vec3f bb_max = p.m_bb.max();

    Algo::Surface::Modelisation::swapVectorMax(bb_min, bb_max);

    for(int i=0; i<3; ++i) {
        if(bb_max[i]-bb_min[i]>0)
            voxel->data()[i] = p.m_voxellisation.getResolution(i)/(bb_max[i]-bb_min[i])*(a[i]-bb_min[i]);
        else
            voxel->data()[i] = 0;


        if(voxel->data()[i] == p.m_voxellisation.getResolution(i))
            --voxel->data()[i];
    }

    return *voxel;
}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(Surface_GenerationCage_Plugin, Surface_GenerationCage_Plugin)
#else
Q_EXPORT_PLUGIN2(Surface_GenerationCage_PluginD, Surface_GenerationCage_Plugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
