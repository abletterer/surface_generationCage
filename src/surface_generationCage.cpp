#include "surface_generationCage.h"

#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

MapParameters::MapParameters() :
    m_initialized(false),
    m_voxellisation(NULL),
    m_resolutions(),
    m_bb(NULL),
    m_dilatation(0),
    m_toVoxellise(true),
    m_independant(true),
    m_extractionFaces(true)
{}

MapParameters::~MapParameters() {}

void MapParameters::start(const QString& mapName, const QString& positionAttributeName) {
    if(!m_initialized) {
        m_initialized = true;
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
    connect(m_generationCageDialog->button_reinitialiserVoxellisation, SIGNAL(clicked()), this, SLOT(reinitialiserVoxellisationFromDialog()));

    connect(m_generationCageDialog->list_maps, SIGNAL(itemSelectionChanged()), this, SLOT(currentMapSelectedChangedFromDialog()));
    connect(m_generationCageDialog->combo_positionAttribute, SIGNAL(currentIndexChanged(QString)), this, SLOT(currentAttributeIndexChangedFromDialog(QString)));
    connect(m_generationCageDialog->check_resolution, SIGNAL(toggled(bool)), this, SLOT(resolutionToggledFromDialog(bool)));
    connect(m_generationCageDialog->spin_resolution_x, SIGNAL(editingFinished()), this, SLOT(resolutionModifiedFromDialog()));
    connect(m_generationCageDialog->spin_resolution_y, SIGNAL(editingFinished()), this, SLOT(resolutionModifiedFromDialog()));
    connect(m_generationCageDialog->spin_resolution_z, SIGNAL(editingFinished()), this, SLOT(resolutionModifiedFromDialog()));
    connect(m_generationCageDialog->radio_extractionFaces, SIGNAL(toggled(bool)), this, SLOT(surfaceExtractionToggledFromDialog(bool)));

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

    disconnect(m_generationCageDialog->list_maps, SIGNAL(itemSelectionChanged()), this, SLOT(currentMapSelectedChangedFromDialog()));
    disconnect(m_generationCageDialog->combo_positionAttribute, SIGNAL(currentIndexChanged(QString)), this, SLOT(currentAttributeIndexChangedFromDialog(QString)));
    disconnect(m_generationCageDialog->check_resolution, SIGNAL(toggled(bool)), this, SLOT(resolutionToggledFromDialog(bool)));
    disconnect(m_generationCageDialog->spin_resolution_x, SIGNAL(editingFinished()), this, SLOT(resolutionModifiedFromDialog()));
    disconnect(m_generationCageDialog->spin_resolution_y, SIGNAL(editingFinished()), this, SLOT(resolutionModifiedFromDialog()));
    disconnect(m_generationCageDialog->spin_resolution_z, SIGNAL(editingFinished()), this, SLOT(resolutionModifiedFromDialog()));
    disconnect(m_generationCageDialog->radio_extractionFaces, SIGNAL(toggled(bool)), this, SLOT(surfaceExtractionToggledFromDialog(bool)));

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

void Surface_GenerationCage_Plugin::reinitialiserVoxellisationFromDialog() {
    QList<QListWidgetItem*> currentItems = m_generationCageDialog->list_maps->selectedItems();
    if(!currentItems.empty())
    {
        const QString& mapName = currentItems[0]->text();

        QString positionName = m_generationCageDialog->combo_positionAttribute->currentText();

        reinitialiserVoxellisation(mapName, positionName);
    }
}

void Surface_GenerationCage_Plugin::currentMapSelectedChangedFromDialog() {
    QList<QListWidgetItem*> currentItems = m_generationCageDialog->list_maps->selectedItems();
    if(!currentItems.empty() && m_generationCageDialog->combo_positionAttribute->currentIndex()!=-1) {
        MapParameters& p = h_parameterSet[currentItems[0]->text()+m_generationCageDialog->combo_positionAttribute->currentText()];
        if(p.m_initialized) {
            m_generationCageDialog->updateAppearanceFromPlugin(p.m_independant, p.m_resolutions[0]!=0);
            m_generationCageDialog->updateResolutionsFromPlugin(p.m_resolutions);
            m_generationCageDialog->updateNiveauDilatationFromPlugin(p.m_dilatation);
        }
        else {
            m_generationCageDialog->updateAppearanceFromPlugin(p.m_independant, false);
        }
    }
}

void Surface_GenerationCage_Plugin::currentAttributeIndexChangedFromDialog(QString nameAttr) {
    QList<QListWidgetItem*> currentItems = m_generationCageDialog->list_maps->selectedItems();
    if(!currentItems.empty() && m_generationCageDialog->combo_positionAttribute->currentIndex()!=-1) {
        MapParameters& p = h_parameterSet[currentItems[0]->text()+nameAttr];
        if(p.m_initialized) {
            m_generationCageDialog->updateAppearanceFromPlugin(p.m_independant, p.m_resolutions[0]!=0);
            m_generationCageDialog->updateResolutionsFromPlugin(p.m_resolutions);
            m_generationCageDialog->updateNiveauDilatationFromPlugin(p.m_dilatation);
        }
        else {
            m_generationCageDialog->updateAppearanceFromPlugin(p.m_independant, false);
        }
    }
}

void Surface_GenerationCage_Plugin::resolutionToggledFromDialog(bool b) {
    QList<QListWidgetItem*> currentItems = m_generationCageDialog->list_maps->selectedItems();
    if(!currentItems.empty() && m_generationCageDialog->combo_positionAttribute->currentIndex()!=-1) {
        MapParameters& p = h_parameterSet[currentItems[0]->text()+m_generationCageDialog->combo_positionAttribute->currentText()];
        if(p.m_initialized) {
            p.m_independant = b;
            m_generationCageDialog->updateAppearanceFromPlugin(p.m_independant, p.m_resolutions[0]!=0);
        }
    }
}

void Surface_GenerationCage_Plugin::resolutionModifiedFromDialog() {
    QList<QListWidgetItem*> currentItems = m_generationCageDialog->list_maps->selectedItems();
    if(!currentItems.empty()) {
        MapParameters& p = h_parameterSet[currentItems[0]->text()+m_generationCageDialog->combo_positionAttribute->currentText()];
        if(p.m_initialized) {
            p.m_toVoxellise = true;
            p.m_resolutions[0] = m_generationCageDialog->spin_resolution_x->text().toInt();
            p.m_resolutions[1] = m_generationCageDialog->spin_resolution_y->text().toInt();
            p.m_resolutions[2] = m_generationCageDialog->spin_resolution_z->text().toInt();
        }
    }
}

void Surface_GenerationCage_Plugin::surfaceExtractionToggledFromDialog(bool b){
    QList<QListWidgetItem*> currentItems = m_generationCageDialog->list_maps->selectedItems();
    if(!currentItems.empty()) {
        MapParameters& p = h_parameterSet[currentItems[0]->text()+m_generationCageDialog->combo_positionAttribute->currentText()];
        p.m_extractionFaces = b;
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

        if(p.m_toCalculateResolutions) {
            calculateResolutions(mapName, positionAttributeName);
        }

        voxellise(mapName, positionAttributeName);
        p.m_voxellisation.marqueVoxelsExterieurs();
        p.m_voxellisation.remplit();
        p.m_dilatation = 0;
    }

    extractionCarte(mapName, positionAttributeName);
    m_generationCageDialog->updateResolutionsFromPlugin(p.m_resolutions);
}

void Surface_GenerationCage_Plugin::dilaterVoxellisation(const QString& mapName, const QString& positionAttributeName) {
    MapParameters& p = h_parameterSet[mapName+positionAttributeName];

    if(p.m_initialized) {
        if(p.m_toVoxellise) {
            MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_schnapps->getMap(mapName));
            PFP2::MAP* selectedMap = mh->getMap();
            VertexAttribute<PFP2::VEC3> position = selectedMap->getAttribute<PFP2::VEC3, VERTEX>(positionAttributeName.toStdString());
            if(!position.isValid()) {
                CGoGNout << "L'attribut de position choisi pour la carte sélectionnée n'est pas valide." << CGoGNendl;
                return;
            }

            voxellise(mapName, positionAttributeName);
        }
        Utils::Chrono chrono;
        chrono.start();
        p.m_voxellisation.dilate();
        CGoGNout << "Temps de dilatation : " << chrono.elapsed() << " ms." << CGoGNendl;
        extractionCarte(mapName, positionAttributeName);
        m_generationCageDialog->updateNiveauDilatationFromPlugin(++p.m_dilatation);
    }
}

void Surface_GenerationCage_Plugin::reinitialiserVoxellisation(const QString& mapName, const QString& positionAttributeName) {
    MapParameters& p = h_parameterSet[mapName+positionAttributeName];

    if(p.m_initialized) {
        p.m_toVoxellise = true;
        generationCage(mapName, positionAttributeName);
        m_generationCageDialog->updateNiveauDilatationFromPlugin(p.m_dilatation);
    }
}

void Surface_GenerationCage_Plugin::extractionCarte(const QString& mapName, const QString& positionAttributeName) {
    MapParameters& p = h_parameterSet[mapName+positionAttributeName];

    if(p.m_initialized) {
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

        Utils::Chrono chrono;
        chrono.start();
        if(p.m_extractionFaces) {
            //Si l'algorithme choisi est celui de l'extraction de faces
            p.m_voxellisation.extractionBord();

            std::vector<std::string> attrNamesCage;
            if(!Algo::Surface::Import::importVoxellisation<PFP2>(*mapCage, p.m_voxellisation, attrNamesCage, true))
            {
                CGoGNerr << "Impossible d'importer la voxellisation" << CGoGNendl ;
                return;
            }

            Algo::Surface::Modelisation::EarTriangulation<PFP2> triang =  Algo::Surface::Modelisation::EarTriangulation<PFP2>(*mapCage);
            triang.triangule();

            CGoGNout << "Temps d'extraction des faces : " << chrono.elapsed() << " ms." << CGoGNendl;

            positionCage = mapCage->getAttribute<PFP2::VEC3, VERTEX>(attrNamesCage[0]);
        }
        else {
            //Si l'algorithme choisi est celui du marching cube
            Algo::Surface::MC::Image<int>* image = p.m_voxellisation.getImage();   //On récupère l'image correspondant à la voxellisation
            Algo::Surface::MC::WindowingEqual<int> windowing;
            windowing.setIsoValue(2); //L'intérieur est représenté avec une valeur de '2'
            Algo::Surface::MC::MarchingCube<int, Algo::Surface::MC::WindowingEqual,PFP2> marching_cube(image, mapCage, positionCage, windowing, false);
            marching_cube.simpleMeshing();
            marching_cube.recalPoints(p.m_bb.min()-Geom::Vec3f(image->getVoxSizeX(), image->getVoxSizeY(), image->getVoxSizeZ()));

            CGoGNout << "Temps de réalisation du Marching Cube : " << chrono.elapsed() << " ms." << CGoGNendl;

            delete image;
            positionCage = mapCage->getAttribute<PFP2::VEC3, VERTEX>("position");
        }

        mh->updateBB(positionCage); //Met a jour la boite englobante de la carte

        mh->notifyConnectivityModification();
        mh->notifyAttributeModification(positionCage);  //Met a jour le VBO
    }
}

void Surface_GenerationCage_Plugin::calculateResolutions(const QString& mapName, const QString& positionAttributeName) {
    MapParameters& p = h_parameterSet[mapName+positionAttributeName];

    if(p.m_initialized) {
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
            n++;
        } while(p.m_resolutions[0]==0 || p.m_resolutions[1]==0 || p.m_resolutions[2]==0);

        CGoGNout << "Initialisation des résolutions : Résolution en x = " << p.m_resolutions[0]
                 << " | Résolution en y = " << p.m_resolutions[1]
                 << " | Résolution en z = " << p.m_resolutions[2] << CGoGNendl;

        m_generationCageDialog->updateResolutionsFromPlugin(p.m_resolutions);

        p.m_toVoxellise = true;
        p.m_toCalculateResolutions = false;
    }
}

/*
  * Fonction qui met à jour les résolutions dans chacun des composantes en fonction de la valeur choisie en x
  */
void Surface_GenerationCage_Plugin::updateResolutions(const QString& mapName, const QString& positionAttributeName) {
    MapParameters& p = h_parameterSet[mapName+positionAttributeName];

    if(p.m_initialized) {
        if(p.m_independant) {
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

        CGoGNout << "Modification des résolutions : Résolution en x = " << p.m_resolutions[0]
                 << " | Résolution en y = " << p.m_resolutions[1]
                 << " | Résolution en z = " << p.m_resolutions[2] << CGoGNendl;

        m_generationCageDialog->updateResolutionsFromPlugin(p.m_resolutions);

        p.m_toVoxellise = true;
        p.m_toCalculateResolutions = false;
    }
}

void Surface_GenerationCage_Plugin::voxellise(const QString& mapName, const QString& positionAttributeName) {
    MapParameters& p = h_parameterSet[mapName+positionAttributeName];

    if(p.m_initialized) {
        Utils::Chrono chrono;
        chrono.start();
        MapHandler<PFP2>* mh = static_cast<MapHandler<PFP2>*>(m_schnapps->getMap(mapName));
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

        p.m_voxellisation = Algo::Surface::Modelisation::Voxellisation(p.m_resolutions, p.m_bb);

        TraversorF<PFP2::MAP> trav_face_map(*selectedMap);
        std::vector<Geom::Vec3i> polygone = std::vector<Geom::Vec3i>();

        for(Dart d = trav_face_map.begin(); d!=trav_face_map.end(); d=trav_face_map.next()) {
            //On traverse l'ensemble des faces de la carte
            Traversor2FV<PFP2::MAP> trav_face_vert(*selectedMap,d);
            polygone.clear();
            for(Dart e = trav_face_vert.begin(); e!=trav_face_vert.end(); e=trav_face_vert.next()) {
                //On récupère les sommets composants la face courante
                if(!voxel[e].isInitialise())
                    voxel[e].setIndexes(getVoxelIndex(mapName, positionAttributeName, position[e]));
                polygone.push_back(voxel[e].getIndexes());
            }
            p.m_voxellisation.voxellisePolygone(polygone);
        }

        p.m_toVoxellise = false;
        m_generationCageDialog->updateAppearanceFromPlugin(p.m_independant, p.m_resolutions[0]!=0);

        CGoGNout << "Temps de voxellisation : " << chrono.elapsed() << " ms" << CGoGNendl;
    }
}

/*
  * Fonction permettant de récupérer les indices du voxel associé au sommet 'a'
  */
Geom::Vec3i& Surface_GenerationCage_Plugin::getVoxelIndex(const QString& mapName, const QString& positionAttributeName, Geom::Vec3f a) {
    MapParameters& p = h_parameterSet[mapName+positionAttributeName];

    Geom::Vec3i* voxel = new Geom::Vec3i();

    if(p.m_initialized) {
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
