#include "TutorialEventRecorder.h"

void TutorialEventRecorder::RecordEvent(const std::string& event)
{
    eventSet_.insert(event);
}

bool TutorialEventRecorder::HasEvent(const std::string& event) const
{
    return eventSet_.contains(event);
}
