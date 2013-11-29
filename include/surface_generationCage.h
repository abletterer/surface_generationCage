#ifndef _SURFACE_GENERATIONCAGE_PLUGIN_H_
#define _SURFACE_GENERATIONCAGE_PLUGIN_H_

#include "plugin_processing.h"

#include "dialog_generationCage.h"

#include "Algo/Modelisation/voxellisation.h"
#include "Algo/Modelisation/triangulation.h"
#include "Algo/MC/marchingcube.h"
#include "Algo/Import/import.h"

#include "Utils/chrono.h"

#include "voxel.h"

namespace CGoGN
{

namespace SCHNApps
{

struct MapCageParameters
{
    MapCageParameters();
    ~MapCageParameters();

    void start();
    void stop();

    bool m_initialized;

    Algo::Surface::Modelisation::Voxellisation m_voxellisation;
    Geom::Vec3i m_resolutions;
    Geom::BoundingBox<PFP2::VEC3> m_bb;
    int m_dilatation;
    bool m_toVoxellise;
    bool m_independant;
    bool m_extractionFaces;
    bool m_toCalculateResolutions;
};

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
    void reinitialiserVoxellisationFromDialog();

    void currentMapSelectedChangedFromDialog();
    void currentAttributeIndexChangedFromDialog(QString nameAttr);

    void resolutionToggledFromDialog(bool b);

    void resolutionXModifiedFromDialog(int value);
    void resolutionYModifiedFromDialog(int value);
    void resolutionZModifiedFromDialog(int value);

    void surfaceExtractionToggledFromDialog(bool b);

public slots:
    MapHandlerGen* generationCage(const QString& mapName, const QString& positionAttributeName);

    void voxellise(const QString& mapName, const QString &positionAttributeName);

    void dilaterVoxellisation(const QString& mapName, const QString& positionAttributeName);
    void reinitialiserVoxellisation(const QString& mapName, const QString& positionAttributeName);

    MapHandlerGen* extractionCarte(const QString& mapName, const QString& positionAttributeName);

    void calculateResolutions(const QString& mapName, const QString& positionAttributeName);
    void updateResolutions(const QString& mapName, const QString& positionAttributeName);

    Geom::Vec3i& getVoxelIndex(const QString& mapName, const QString& positionAttributeName, Geom::Vec3f a);
private:
    Dialog_GenerationCage* m_generationCageDialog;
    QAction* m_generationCageAction;

public:
    QHash<QString, MapCageParameters> h_parameterSet;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif

