#ifndef _SURFACE_GENERATIONCAGE_PLUGIN_H_
#define _SURFACE_GENERATIONCAGE_PLUGIN_H_

#include "plugin_processing.h"

#include "dialog_generationCage.h"

#include "Algo/Modelisation/voxellisation.h"
#include "Algo/MC/marchingcube.h"
#include "Algo/Import/import.h"

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

    void calculateResolutions();
    Geom::Vec3i& updateResolutions(bool independant);

    void voxellise();

    Geom::Vec3i& getVoxelIndex(Geom::Vec3f a);
private:
    Dialog_GenerationCage* m_generationCageDialog;
    QAction* m_generationCageAction;

    /*struct GenerationCageParameters
    {
        GenerationCageParameters() {}
        GenerationCageParameters(
            Algo::Surface::Modelisation::Voxellisation& voxellisation,
            Geom::Vec3i resolutions, Geom::BoundingBox<PFP2::VEC3> bb) :
            m_voxellisation(voxellisation), m_resolutions(resolutions), m_bb(bb)
        {}
        Algo::Surface::Modelisation::Voxellisation m_voxellisation;
        Geom::Vec3i m_resolutions;
        Geom::BoundingBox<PFP2::VEC3> m_bb;
    };
    QHash<QString, GenerationCageParameters> generationCageParameters;*/

    Algo::Surface::Modelisation::Voxellisation m_voxellisation;
    Geom::Vec3i m_resolutions;
    Geom::BoundingBox<PFP2::VEC3> m_bb;

    bool m_voxellisationNeeded;


};

} // namespace SCHNApps

} // namespace CGoGN

#endif

