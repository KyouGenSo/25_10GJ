#pragma once
#include <string>
#include <set>

class TutorialEventRecorder
{
public:
    TutorialEventRecorder() = default;
    ~TutorialEventRecorder() = default;

    void RecordEvent(const std::string& event);
    bool HasEvent(const std::string& event) const;
    void ResetEvent(const std::string& event) { eventSet_.erase(event); }


private:
    std::set<std::string> eventSet_;
};