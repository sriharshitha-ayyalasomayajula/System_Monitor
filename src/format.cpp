#include "format.h"
#include <iomanip>
#include <string>
#include <iostream>
using namespace std;

ostringstream stream;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function

string Format::ElapsedTime(long time) {
  int const secondsinDay = 86400;
  int const secondsinHours = 3600;
  int const secondsinMinutes = 60;

  int elapseDays = (time / secondsinDay);
  int elapseHours = (time - (elapseDays * secondsinDay)) / secondsinHours;
  int elapseMinute =
      (time - (elapseDays * secondsinDay) - (elapseHours * secondsinHours)) /
      secondsinMinutes;
  int elapseSeconds = static_cast<int>(time - (elapseDays * secondsinDay) -
                                       (elapseHours * secondsinHours) -
                                       (elapseMinute * secondsinMinutes));

  string hours =  to_string(elapseHours);
  string minutes = to_string(elapseMinute);
  string seconds = to_string(elapseSeconds);

  return hours.insert(0, 2 - hours.length(), '0') +":" + minutes.insert(0, 2 - minutes.length(), '0') + ":" + seconds.insert(0, 2 - seconds.length(), '0');
         
}