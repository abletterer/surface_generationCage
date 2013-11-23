#include "surface_generationCage.h"

#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

bool Surface_GenerationCage_Plugin::enable()
{
    m_generationCageDialog = new Dialog_GenerationCage(m_schnapps);

    m_generationCageAction = new QAction("Generate cage", this);

    m_voxellisation = new Algo::Surface::Modelisation::Voxellisation();

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
    if(nameAttr == m_generationCageDialog->combo_positionAttribute->currentText()) {
        //Si l'attribut modifié correspond à celui utilisé actuellement pour réaliser les calculs
        m_generationCageDialog->setAttributePositionChanged(true);
    }
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

        bool autoUpdate = (currentItems[0]->checkState() == Qt::Checked);

        generationCage(mapName, positionName);
    }
}

void Surface_GenerationCage_Plugin::dilaterVoxellisationFromDialog() {
    QList<QListWidgetItem*> currentItems = m_generationCageDialog->list_maps->selectedItems();
    if(!currentItems.empty())
    {
        const QString& mapName = currentItems[0]->text();

        QString positionName = m_generationCageDialog->combo_positionAttribute->currentText();

        bool autoUpdate = (currentItems[0]->checkState() == Qt::Checked);

        dilaterVoxellisation(mapName, positionName);
    }
}

void Surface_GenerationCage_Plugin::generationCage(const QString& mapName, const QString& positionAttributeName) {
    if(m_generationCageDialog->isAttributePositionChanged()) {
        //Si depuis les derniers traitements l'attribut de position a été modifié
    }
    if(m_generationCageDialog->radio_extractionFaces->isChecked()) {
        //Si l'algorithme choisi est celui de l'extraction de faces
    }
    else if(m_generationCageDialog->radio_marchingCube->isChecked()){
        //Si l'algorithme choisi est celui du marching cube
    }
}

void Surface_GenerationCage_Plugin::dilaterVoxellisation(const QString& mapName, const QString& positionAttributeName) {
    if(m_generationCageDialog->isAttributePositionChanged()) {
        //Si depuis les derniers traitements l'attribut de position a été modifié
        m_generationCageDialog->setAttributePositionChanged(false);
    }
}

Geom::Vec3i& Surface_GenerationCage_Plugin::calculateResolutions(const Geom::BoundingBox<PFP2::VEC3>& bb) {
    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_schnapps->getSelectedMap());
    PFP2::MAP* selectedMap = mh->getMap();

    Geom::Vec3f bb_min = bb.min();
    Geom::Vec3f bb_max = bb.max();

    Geom::Vec3i* resolutions = new Geom::Vec3i();

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
        resolutions[0] = n*delta_x/max;
        resolutions[1] = n*delta_y/max;
        resolutions[2] = n*delta_z/max;
        n+=5;
    } while(resolutions[0]==0 || resolutions[1]==0 || resolutions[2]==0);

    CGoGNout << "Initialisation des résolutions : Résolution en x = " << resolutions[0] << " | Résolution en y = "<< resolutions[1] << " | Résolution en z = " << resolutions[2] << CGoGNendl;

    return *resolutions;
}

/*
  * Fonction qui met à jour les résolutions dans chacun des composantes en fonction de la valeur choisie en x
  */
Geom::Vec3i& Surface_GenerationCage_Plugin::updateResolutions(int res_x, const Geom::BoundingBox<PFP2::VEC3>& bb) {
    Geom::Vec3i* resolutions  = new Geom::Vec3i();
    Geom::Vec3f bb_min = bb.min();
    Geom::Vec3f bb_max = bb.max();

    Algo::Surface::Modelisation::swapVectorMax(bb_min, bb_max);

    float delta_x = bb_max[0]-bb_min[0];
    float delta_y = bb_max[1]-bb_min[1];
    float delta_z = bb_max[2]-bb_min[2];

    float max = std::max(std::max(delta_x, delta_y), delta_z);  //On récupère la composante qui a l'écart maximum

    if(res_x<=0)
        res_x = 1;

    int n = res_x*max/delta_x;

    //On adapte la résolution calculée pour qu'elle soit différente dans chacune des composantes x, y et z
    resolutions[0] = n*delta_x/max;
    resolutions[1] = n*delta_y/max;
    resolutions[2] = n*delta_z/max;

    CGoGNout << "Modification des résolutions : Résolution en x = " << resolutions[0] << " | Résolution en y = "<< resolutions[1] << " | Résolution en z = " << resolutions[2] << CGoGNendl;

    return *resolutions;
}

void Surface_GenerationCage_Plugin::voxellise() {
    delete m_voxellisation;

    MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_schnapps->getSelectedMap());
    PFP2::MAP* selectedMap = mh->getMap();

    QString positionAttributeName = m_generationCageDialog->combo_positionAttribute->currentText();
    VertexAttribute<PFP2::VEC3> position = selectedMap->getAttribute<PFP2::VEC3, VERTEX>(positionAttributeName.toStdString());
    if(!position.isValid()) {
        CGoGNout << "La position n'est pas valide pour la carte sélectionnée." << CGoGNendl;
        return;
    }

    VertexAttribute<Voxel> voxel = selectedMap->getAttribute<Voxel, VERTEX>("voxel");
    if(!voxel.isValid()) {
        voxel = selectedMap->addAttribute<Voxel, VERTEX>("voxel");
    }

    Geom::BoundingBox<PFP2::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP2>(*selectedMap, position) ;
    Geom::Vec3i resolutions = calculateResolutions(bb);

    m_voxellisation = new Algo::Surface::Modelisation::Voxellisation(resolutions[0], resolutions[1], resolutions[2], bb);

    TraversorF<PFP2::MAP> trav_face_map(*selectedMap);
    std::vector<Geom::Vec3i> polygone = std::vector<Geom::Vec3i>();

    for(Dart d = trav_face_map.begin(); d!=trav_face_map.end(); d=trav_face_map.next()) {
        //On traverse l'ensemble des faces de la carte
        Traversor2FV<PFP2::MAP> trav_face_vert(*selectedMap,d);
        polygone.clear();
        for(Dart e = trav_face_vert.begin(); e!=trav_face_vert.end(); e=trav_face_vert.next()) {
            //On récupère les sommets composants la face courante
            voxel[e].setIndexes(getVoxelIndex(position[e], bb));
            polygone.push_back(voxel[e].getIndexes());
        }
        m_voxellisation->voxellisePolygone(polygone);
    }

    CGoGNout << "Fin de la voxellisation. Il y a " << m_voxellisation->size() << " voxel(s) qui entourent le maillage" << CGoGNendl;
}

/*
  * Fonction permettant de récupérer les indices du voxel associé au sommet 'a'
  */
Geom::Vec3i& Surface_GenerationCage_Plugin::getVoxelIndex(Geom::Vec3f a, const Geom::BoundingBox<PFP2::VEC3>& bb) {
    Geom::Vec3i* voxel = new Geom::Vec3i();

    Geom::Vec3f bb_min = bb.min();
    Geom::Vec3f bb_max = bb.max();

    Algo::Surface::Modelisation::swapVectorMax(bb_min, bb_max);

    for(int i=0; i<3; ++i) {
        if(bb_max[i]-bb_min[i]>0)
            voxel->data()[i] = m_voxellisation->getResolution(i)/(bb_max[i]-bb_min[i])*(a[i]-bb_min[i]);
        else
            voxel->data()[i] = 0;


        if(voxel[i] == m_voxellisation->getResolution(i))
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
