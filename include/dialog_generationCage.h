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

class Dialog_GenerationCage : public QDialog, public Ui::Dialog_GenerationCage
{
    Q_OBJECT

public:
    Dialog_GenerationCage(SCHNApps* s);

    bool isVoxellisationNeeded() { return m_voxellisationNeeded; }
    void setVoxellisationNeeded(bool needed) { m_voxellisationNeeded = needed; }

    void updateResolutionsSpinsValues(const Geom::Vec3i* resolutions) {
        spin_resolution_x->setValue(resolutions->data()[0]);
        spin_resolution_y->setValue(resolutions->data()[1]);
        spin_resolution_z->setValue(resolutions->data()[2]);
    }

private:
    SCHNApps* m_schnapps;
    MapHandlerGen* m_selectedMap;

    bool m_voxellisationNeeded;

public slots:
    void addMapToList(MapHandlerGen* m);
    void removeMapFromList(MapHandlerGen* m);
    void addAttributeToList(unsigned int orbit, const QString& nameAttr);

    void selectedMapChanged();
    void selectedPositionAttributeChanged(QString positionName);
    void resolutionTriggered(bool b);
    void resolutionChanged();
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
