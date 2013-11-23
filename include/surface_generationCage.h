#ifndef _SURFACE_GENERATIONCAGE_PLUGIN_H_
#define _SURFACE_GENERATIONCAGE_PLUGIN_H_

#include "plugin_processing.h"

#include "dialog_generationCage.h"

#include "Algo/Modelisation/voxellisation.h"

#include "voxel.h"

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
    void dilaterVoxellisationFromDialog();

    void generationCage(const QString& mapName, const QString& positionAttributeName);
    void dilaterVoxellisation(const QString& mapName, const QString& positionAttributeName);

    void calculateResolutions(const Geom::BoundingBox<PFP2::VEC3>& bb, bool first=true);
    Geom::Vec3i& updateResolutions(const Geom::BoundingBox<PFP2::VEC3>& bb, bool independant=false);

    void voxellise();

    Geom::Vec3i& getVoxelIndex(Geom::Vec3f a, const Geom::BoundingBox<PFP2::VEC3>& bb);
private:
    Dialog_GenerationCage* m_generationCageDialog;
    QAction* m_generationCageAction;
    Algo::Surface::Modelisation::Voxellisation* m_voxellisation;
    Geom::Vec3i* m_resolutions;

    bool m_voxellisationNeeded;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif

