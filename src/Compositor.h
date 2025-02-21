#pragma once

#include <LCompositor.h>
#include <LLayerView.h>
#include <LSceneView.h>

#include "scene/Scene.h"
#include "utils/Assets.h"
#include "utils/Systemd.h"
#include "Surface.h"

#include <memory>

using namespace Louvre;

class Assets;

class Compositor final : public LCompositor {
public:
    /* A single scene to drive all outputs and input events */
    Scene scene;
    void focusNextMonitor() noexcept;
    void focusPrevMonitor() noexcept;
    inline int monitorIndex() const noexcept { return m_monitor_index; }


/* Cursors, textures, etc */
std::unique_ptr<Assets> assets;

std::unique_ptr<Systemd> systemd;
protected:

/* Triggered after calling start() from main.cpp */
void initialized() override;

/* Triggered after calling finish(), see Keyboard */
void uninitialized() override;

/* Object factory, used to override Louvre default classes */
LFactoryObject *createObjectRequest(LFactoryObject::Type objectType, const void *params) override;
void onAnticipatedObjectDestruction(LFactoryObject *object) override;

/* If you want to add/remove protocol globals */
bool createGlobalsRequest() override;

/* Controls which client can bind to which global */
bool globalsFilter(LClient *client, LGlobal *global) override;

private:
    int m_monitor_index;
};
