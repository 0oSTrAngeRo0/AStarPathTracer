#ifndef PATHTRACER_DEBUGGER_H
#define PATHTRACER_DEBUGGER_H


#include "event-registry.h"
#include "vulkan-app.h"

class Debugger: public IEventHandler<VulkanApp::OnDebug>{
public:
    void OnExecute(VulkanApp::OnDebug event) override;
};


#endif //PATHTRACER_DEBUGGER_H
