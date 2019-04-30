#ifndef VALIDATOR_H
#define VALIDATOR_H


#include <string>
#include <vector>
#include <fstream>
#include <iostream>

// BEGIN OF HEADER

namespace validator {

class Course
{
  friend std::istream& operator>>(std::istream&, Course&);
public:
  const std::string& Name() const { return name; }
  const std::string& Teacher() const { return teacher; }
  unsigned Students() const { return students; }
  unsigned Lectures() const { return lectures; }
  unsigned MinWorkingDays() const { return min_working_days; }
private:
  std::string name, teacher;
  unsigned lectures, students, min_working_days;
};

class Curriculum
{
public:
  const std::string& Name() const { return name; }
  void SetName(const std::string& n) { name = n; }
  unsigned Size() const { return members.size(); }
  void AddMember(unsigned e) { members.push_back(e); }
  unsigned operator[](unsigned i) const { return members[i]; }
private:
  std::string name;
  std::vector<unsigned> members;
};

class Room
{
  friend std::istream& operator>>(std::istream&, Room&);
public:
  const std::string& Name() const { return name; }
  unsigned Capacity() const { return capacity; }
private:
  std::string name;
  unsigned capacity;
};

class Faculty
{
public:
  Faculty(const std::string& instance);

  unsigned Courses() const { return courses; }
  unsigned Rooms() const { return rooms; }
  unsigned Curricula() const { return curricula; }
  unsigned Periods() const { return periods; }
  unsigned PeriodsPerDay() const { return periods_per_day; }
  unsigned Days() const { return periods/periods_per_day; }

  bool  Available(unsigned c, unsigned p) const { return availability[c][p]; }
  bool Conflict(unsigned c1, unsigned c2) const { return conflict[c1][c2]; }
  const Course& CourseVector(int i) const { return course_vect[i]; }
  const Room& RoomVector(int i) const { return room_vect[i]; }
  const Curriculum& CurriculaVector(int i) const { return curricula_vect[i]; }

  bool CurriculumMember(unsigned c, unsigned g) const;

  int RoomIndex(const std::string&) const;
  int CourseIndex(const std::string&) const;
  int CurriculumIndex(const std::string&) const;
  int PeriodIndex(const std::string&) const;
  const std::string& Name() const { return name; }

  const unsigned MIN_WORKING_DAYS_COST;
  const unsigned CURRICULUM_COMPACTNESS_COST;
  const unsigned ROOM_STABILITY_COST;

private:
  // scalar data
  std::string name;
  unsigned rooms, courses, periods, periods_per_day, curricula;

  // data objects
  std::vector<Course> course_vect;
  std::vector<Room> room_vect;
  std::vector<Curriculum> curricula_vect;

  // availability and conflicts constraints
  std::vector<std::vector<bool> > availability;
  std::vector<std::vector<bool> > conflict;
};

class Timetable
{
public:
  Timetable(const Faculty & f, const std::string file_name);
  // Inspect timetable
  unsigned operator()(unsigned i, unsigned j) const { return tt[i][j]; }
  unsigned& operator()(unsigned i, unsigned j) { return tt[i][j]; }
  // Inspect redundant data
  unsigned RoomLectures(unsigned i, unsigned j) const { return room_lectures[i][j]; }
  unsigned CurriculumPeriodLectures(unsigned i, unsigned j) const { return curriculum_period_lectures[i][j]; }
  unsigned CourseDailyLectures(unsigned i, unsigned j) const { return course_daily_lectures[i][j]; }
  unsigned WorkingDays(unsigned i) const { return working_days[i]; }
  unsigned UsedRoomsNo(unsigned i) const { return used_rooms[i].size(); }
  unsigned UsedRooms(unsigned i, unsigned j) const { return used_rooms[i][j]; }
  void InsertUsedRoom(unsigned i, unsigned j) { used_rooms[i].push_back(j); }
  unsigned Warnings() const { return warnings; }
  void UpdateRedundantData();

 private:
  const Faculty & in;
  unsigned warnings;
  std::vector<std::vector<unsigned> > tt;  // (courses X periods) timetable matrix
    // redundant data
  std::vector<std::vector<unsigned> > room_lectures; // number of lectures per room in the same period (should be 0 or 1)
  std::vector<std::vector<unsigned> > curriculum_period_lectures; // number of lectures per curriculum in the same period (should be 0 or 1)
  std::vector<std::vector<unsigned> > course_daily_lectures; // number of lectures per course per day
  std::vector<unsigned> working_days; // number of days of lecture per course
  std::vector<std::vector<unsigned> > used_rooms; // rooms used for each lecture on the course
};



class Validator
{
public:
  Validator(const Faculty & i, const Timetable & o)
    : in(i), out(o) {}
  void PrintCosts(std::ostream& os) const;
  void PrintTotalCost(std::ostream& os) const;
  void PrintViolations(std::ostream& os) const;
private:
  unsigned CostsOnLectures() const;
  unsigned CostsOnConflicts() const;
  unsigned CostsOnAvailability() const;
  unsigned CostsOnRoomOccupation() const;
  unsigned CostsOnRoomCapacity() const;
  unsigned CostsOnMinWorkingDays() const;
  unsigned CostsOnCurriculumCompactness() const;
  unsigned CostsOnRoomStability() const;

  void PrintViolationsOnLectures(std::ostream& os) const;
  void PrintViolationsOnConflicts(std::ostream& os) const;
  void PrintViolationsOnAvailability(std::ostream& os) const;
  void PrintViolationsOnRoomOccupation(std::ostream& os) const;
  void PrintViolationsOnRoomCapacity(std::ostream& os) const;
  void PrintViolationsOnMinWorkingDays(std::ostream& os) const;
  void PrintViolationsOnCurriculumCompactness(std::ostream& os) const;
  void PrintViolationsOnRoomStability(std::ostream& os) const;

  const Faculty& in;
  const Timetable& out;
};


}

// END OF HEADER


#endif // VALIDATOR_H
