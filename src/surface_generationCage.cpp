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
    if(nameAttr==m_generationCageDialog->combo_positionAttribute->currentText()) {
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

        dilaterVoxellisation(mapName);
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

void Surface_GenerationCage_Plugin::dilaterVoxellisation(const QString& mapName) {
    if(m_generationCageDialog->isAttributePositionChanged()) {
        //Si depuis les derniers traitements l'attribut de position a été modifié
        m_generationCageDialog->setAttributePositionChanged(false);
    }
}

void Surface_GenerationCage_Plugin::calculateResolutions(const QString& mapName) {

}

#ifndef DEBUG
Q_EXPORT_PLUGIN2(Surface_GenerationCage_Plugin, Surface_GenerationCage_Plugin)
#else
Q_EXPORT_PLUGIN2(Surface_GenerationCage_PluginD, Surface_GenerationCage_Plugin)
#endif

} // namespace SCHNApps

} // namespace CGoGN
