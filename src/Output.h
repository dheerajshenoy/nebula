#pragma once
#ifndef OUTPUT_H
#define OUTPUT_H

#include <LOutput.h>
#include <LSolidColorView.h>
#include <vector>
#include <LLayerView.h>
#include <LTextureView.h>
#include <LSceneView.h>
#include "scene/Scene.h"

class LayoutManager;
class Workspace;
class Surface;

using namespace Louvre;

class Output final : public LOutput {
public:
    Output(const void * params);
    ~Output();
    void initializeGL() override;
    void paintGL() override;
    void moveGL() override;
    void resizeGL() override;
    void uninitializeGL() override;
    void setGammaRequest(LClient *client, const LGammaTable *gamma) override;
    void availableGeometryChanged() override;
    LSolidColorView fadeInView { {0.f, 0.f, 0.f } };
    Surface *searchFullscreenSurface() const noexcept;
    bool tryDirectScanout(Surface *surface) noexcept;
    void setWorkspace(const size_t &n) noexcept;
    LayoutManager* layoutManager() noexcept;
    inline Workspace* workspace() const noexcept { return m_workspaces.at(m_workspace_index); }
    LLayerView workspacesContainer;

    void zoomedDrawBegin() noexcept;
    void zoomedDrawEnd() noexcept;
    void setZoom(Float32 zoom) noexcept;
    Float32 zoom { 1.f };
    LSceneView zoomScene { LSize(0, 0), 1.f };
    LTextureView zoomView;


private:
    std::vector<Workspace*> m_workspaces;
    int m_workspace_index;
};

#endif // OUTPUT_H
