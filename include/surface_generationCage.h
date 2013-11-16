#ifndef _SURFACE_GENERATIONCAGE_PLUGIN_H_
#define _SURFACE_GENERATIONCAGE_PLUGIN_H_

#include "plugin_processing.h"

#include "dialog_generationCage.h"

namespace CGoGN
{

namespace SCHNApps
{

class Surface_GenerationCage_Plugin : public PluginProcessing
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)

public:
    Surface_GenerationCage_Plugin()
	{}

    ~Surface_GenerationCage_Plugin()
	{}

	virtual bool enable();
	virtual void disable();

private slots:
	void mapAdded(MapHandlerGen* map);
	void mapRemoved(MapHandlerGen* map);
    void attributeModified(unsigned int orbit, QString nameAttr);

    void openGenerationCageDialog();
    void generationCageFromDialog();

    void generationCage();
private:
    Dialog_GenerationCage* m_generationCageDialog;
    QAction* m_generationCageAction;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif

