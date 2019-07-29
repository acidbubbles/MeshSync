#pragma once
#include <iostream>
#include <list>
#include <mutex>
#include <future>
#include "SceneGraph/msSceneGraph.h"

namespace ms {

enum class SceneCacheEncoding
{
    Plain,
    ZSTD,
};

union SceneCacheEncoderSettings
{
    struct {
        int compression_level;
    } zstd;
};

struct TimeRange
{
    float start = 0.0f;
    float end = 0.0f;
};

struct OSceneCacheSettingsBase
{
    SceneCacheEncoding encoding = SceneCacheEncoding::ZSTD;
    SceneCacheEncoderSettings encoder_settings;

    // flags
    uint32_t strip_unchanged : 1;
    uint32_t apply_refinement : 1;
    uint32_t flatten_hierarchy : 1;
    uint32_t merge_meshes : 1; // todo
    uint32_t strip_normals : 1;
    uint32_t strip_tangents : 1;

    OSceneCacheSettingsBase();
};

struct OSceneCacheSettings : OSceneCacheSettingsBase, SceneImportSettings {};

class OSceneCache
{
public:
    virtual ~OSceneCache() {}
    virtual bool valid() const = 0;

    // note:
    // *scene will be modified* if scene optimization options (strip_unchanged, apply_refinement, etc) are enabled.
    // pass cloned scene (Scene::clone()) if you need to keep source scenes intact.
    virtual void addScene(ScenePtr scene, float time) = 0;

    virtual void flush() = 0;
    virtual bool isWriting() = 0;
};
msDeclPtr(OSceneCache);


struct ISceneCacheSettingsBase
{
    uint32_t convert_scenes : 1;
    uint32_t enable_diff : 1;
    uint32_t preload_scenes : 1; // todo
    uint32_t preload_entire_file : 1;
    uint32_t generate_velocities : 1;
    int max_history = 2;

    SceneImportSettings sis;

    ISceneCacheSettingsBase();
};
struct ISceneCacheSettings : ISceneCacheSettingsBase, SceneImportSettings {};

class ISceneCache
{
public:
    virtual ~ISceneCache() {}
    virtual bool valid() const = 0;

    virtual TimeRange getTimeRange() const = 0;
    virtual size_t getNumScenes() const = 0;
    virtual ScenePtr getByIndex(size_t i) = 0;
    virtual ScenePtr getByTime(float t, bool lerp) = 0;
};
msDeclPtr(ISceneCache);


OSceneCachePtr OpenOSceneCacheFile(const char *path, const OSceneCacheSettings& oscs = OSceneCacheSettings());
OSceneCache* OpenOSceneCacheFileRaw(const char *path, const OSceneCacheSettings& oscs = OSceneCacheSettings());

ISceneCachePtr OpenISceneCacheFile(const char *path, const ISceneCacheSettings& iscs = ISceneCacheSettings());
ISceneCache* OpenISceneCacheFileRaw(const char *path, const ISceneCacheSettings& iscs = ISceneCacheSettings());


} // namespace ms