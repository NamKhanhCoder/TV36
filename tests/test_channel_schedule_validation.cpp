// The group build includes every .cpp. Enable this standalone test explicitly
// with -DTV36_CHANNEL_SCHEDULE_TEST and compile it without the app's main.cpp.
#ifdef TV36_CHANNEL_SCHEDULE_TEST

#include "Entities/Channel.h"
#include "Entities/Schedule.h"
#include <iostream>
#include <stdexcept>

// Call entities directly: these checks must work without InputHelper or a menu.
template<class EntityType, class Action>
void rejectsWithoutChanging(EntityType& item, Action action)
{
    const std::string before = item.exportToString();
    bool rejected = false;
    try { action(); }
    catch (const std::invalid_argument&) { rejected = true; }
    catch (const std::out_of_range&) { rejected = true; }
    if (!rejected || item.exportToString() != before)
        throw std::runtime_error("Invalid input was accepted or changed the entity.");
}

int main()
{
    Channel channel("KENH001", "News", "TL001", 1, "HD", "https://example.com/live.m3u8", "DANG_PHAT");
    rejectsWithoutChanging(channel, [&] { channel.setName("   "); });
    rejectsWithoutChanging(channel, [&] { channel.setCategoryID("khenh"); });
    rejectsWithoutChanging(channel, [&] { channel.setChannelNumber(0); });
    rejectsWithoutChanging(channel, [&] { channel.setResolution("BAD"); });
    rejectsWithoutChanging(channel, [&] { channel.setStreamURL("url|broken"); });
    rejectsWithoutChanging(channel, [&] { channel.setStatus("2"); });
    for (const auto& row : {
        "KENH001|News",
        "BAD001|News|TL001|1|HD|url|DANG_PHAT",
        "KENH001|News|TL001|1abc|HD|url|DANG_PHAT",
        "KENH001|News|TL001|999999999999999999999|HD|url|DANG_PHAT",
        "KENH001|News|TL001|1|HD|url|BAD"})
        rejectsWithoutChanging(channel, [&] { channel.parseFromString(row); });

    Schedule schedule("LPS001", "KENH001", "News", "2026-10-21 19:00", "2026-10-21 20:00", "");
    rejectsWithoutChanging(schedule, [&] { schedule.setChannelID("BAD001"); });
    rejectsWithoutChanging(schedule, [&] { schedule.setProgramName(""); });
    rejectsWithoutChanging(schedule, [&] { schedule.setDescription("bad\ntext"); });
    rejectsWithoutChanging(schedule, [&] { schedule.setStartTime("2026-10-21 20:00"); });
    rejectsWithoutChanging(schedule, [&] { schedule.setEndTime("2026-10-21 18:00"); });
    rejectsWithoutChanging(schedule, [&] { schedule.setTimeRange("2026-10-22 19:00", "2026-10-22 18:00"); });
    for (const auto& row : {
        "LPS001|KENH001",
        "LPS001|KENH001|News|2026-02-29 19:00|2026-03-01 20:00|",
        "LPS001|KENH001|News|2026-10-21 24:00|2026-10-22 01:00|",
        "LPS001|KENH001|News|2026-10-21 19:00|2026-10-21 19:00|"})
        rejectsWithoutChanging(schedule, [&] { schedule.parseFromString(row); });

    schedule.setTimeRange("2028-02-29 23:00", "2028-03-01 01:00");
    Channel loadedChannel;
    Schedule loadedSchedule;
    loadedChannel.parseFromString(channel.exportToString());
    loadedSchedule.parseFromString(schedule.exportToString());
    if (loadedChannel.exportToString() != channel.exportToString() ||
        loadedSchedule.exportToString() != schedule.exportToString())
        throw std::runtime_error("Valid entity did not round-trip.");
    std::cout << "PASS: direct setter/parser rejection, unchanged state, leap day and round-trip.\n";
}

#endif
