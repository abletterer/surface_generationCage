#ifndef _DIALOG_GENERATI0NCAGE_H_
#define _DIALOG_GENERATIONCAGE_H_

#include "ui_dialog_generationCage.h"

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

    bool isAttributePositionChanged() { return m_attributePositionChanged; }
    void setAttributePositionChanged(bool changed) { m_attributePositionChanged = changed; }

private:
    SCHNApps* m_schnapps;
    MapHandlerGen* m_selectedMap;

    bool m_attributePositionChanged;

public slots:
    void selectedMapChanged();
    void selectedPositionAttributeChanged(QString positionName);
    void addMapToList(MapHandlerGen* m);
    void removeMapFromList(MapHandlerGen* m);
    void addAttributeToList(unsigned int orbit, const QString& nameAttr);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
