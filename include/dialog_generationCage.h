#ifndef _DIALOG_GENERATI0NCAGE_H_
#define _DIALOG_GENERATIONCAGE_H_

#include "ui_dialog_generationCage.h"

#include "Geometry/vector_gen.h"

namespace CGoGN
{

namespace SCHNApps
{

class SCHNApps;
class MapHandlerGen;
class Surface_GenerationCage_Plugin;
class MapParameters;

class Dialog_GenerationCage : public QDialog, public Ui::Dialog_GenerationCage
{
    Q_OBJECT

public:
    Dialog_GenerationCage(SCHNApps* s);

    void updateResolutionsFromPlugin(Geom::Vec3i resolutions) {
        spin_resolution_x->setValue(resolutions[0]);
        spin_resolution_y->setValue(resolutions[1]);
        spin_resolution_z->setValue(resolutions[2]);
    }

    void updateNiveauDilatationFromPlugin(int dilatation) {
        line_niveauDilatation->setText(QString::number(dilatation));
    }

    /*
      * Fonction qui met  jour l'apparence de l'interface en fonction de la configuration de chaque carte
      */
    void updateAppearanceFromPlugin(bool independant, bool initialized) {
        group_resolution->setEnabled(initialized);
        group_generate->setEnabled(initialized || list_maps->currentRow()!=-1);
        group_dilate->setEnabled(initialized);
        check_resolution->setChecked(independant);
        spin_resolution_y->setEnabled(independant);
        spin_resolution_z->setEnabled(independant);
    }

private:
    SCHNApps* m_schnapps;
    MapHandlerGen* m_selectedMap;

public slots:
    void addMapToList(MapHandlerGen* m);
    void removeMapFromList(MapHandlerGen* m);
    void addAttributeToList(unsigned int orbit, const QString& nameAttr);

    void selectedMapChanged();
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
