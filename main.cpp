// This is a test of C++ Boost library multi-index container

#include <iostream>
#include <string>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/key.hpp>



int main (int argc, char *argv[]) {
  // a mock class for data Person
  struct Person {
    int id;
    std::string name;
    int age;
    std::string nickname;
    std::string language;
  };

  // Define a multi-index container 

  // tag struct for accessing the index
  struct by_id {};
  struct by_name {};
  struct by_language_nickname {};
  struct by_age_nickname {};

  typedef boost::multi_index_container<
    Person*,
    boost::multi_index::indexed_by<
      boost::multi_index::hashed_unique<
        boost::multi_index::tag<by_name>,
        boost::multi_index::key<&Person::name>,
        std::hash<std::string>
      >,
      boost::multi_index::hashed_unique<
        boost::multi_index::tag<by_id>,
        boost::multi_index::key<&Person::id>,
        std::hash<int>
      >,
      boost::multi_index::hashed_unique<
        boost::multi_index::tag<by_language_nickname>,
        boost::multi_index::composite_key<
          Person,
          boost::multi_index::key<&Person::name>,
          boost::multi_index::key<&Person::nickname>
        >,
        boost::multi_index::composite_key_hash<
          std::hash<std::string>,
          std::hash<std::string>
        >
      >,
      boost::multi_index::hashed_unique<
        boost::multi_index::tag<by_age_nickname>,
        boost::multi_index::composite_key<
          Person,
          boost::multi_index::key<&Person::age>,
          boost::multi_index::key<&Person::nickname>
        >,
        boost::multi_index::composite_key_hash<
          std::hash<int>,
          std::hash<std::string>
        >
      >
    > 
  > PersonMultiIndexContainer;

  /*Person p1 = {1, "Alice", 20, "Ali", "English"};*/
  /*Person p2 = {2, "Bob", 30, "Bobby", "French"};*/
  /*Person p3 = {3, "Cathy", 40, "Cat", "Spanish"};*/
  /*Person p4 = {4, "David", 50, "Dave", "German"};*/

  /*std::vector<Person*> persons_vec = {&p1, &p2, &p3, &p4};*/
  // vector of unique pointers
  std::vector<std::unique_ptr<Person>> persons_vec;
  persons_vec.push_back(std::make_unique<Person>(Person{1, "Alice", 20, "Ali", "English"}));
  persons_vec.push_back(std::make_unique<Person>(Person{2, "Bob", 30, "Bobby", "French"}));
  persons_vec.push_back(std::make_unique<Person>(Person{3, "Cathy", 40, "Cat", "Spanish"}));    
  persons_vec.push_back(std::make_unique<Person>(Person{4, "David", 50, "Dave", "German"}));

  PersonMultiIndexContainer persons;
  /*persons.insert(&p1);*/
  /*persons.insert(&p2);*/
  /*persons.insert(&p3);*/
  /*persons.insert(&p4);*/
  for (auto& p : persons_vec) {
    persons.insert(p.get());
  }

  // Query data from the container
  
  // Query by name
  auto& name_index = persons.get<by_name>();
  for (auto& p : persons_vec) {
    auto it = name_index.find(p->name);
    if (it != name_index.end()) {
      std::cout << "Found: " << (*it)->name << std::endl;
    } else {
      std::cout << "Not found" << std::endl;
    }
  }
  std::cout << std::endl;
  

  // Query by by_id
  auto& id_index = persons.get<by_id>();
  auto it2 = id_index.find(3);
  if (it2 != id_index.end()) {
    std::cout << "Found: " << (*it2)->name << std::endl;
  } else {
    std::cout << "Not found" << std::endl;
  }

  // Query by language and by_language_nickname
  auto& language_nickname_index = persons.get<by_language_nickname>();
  auto it3 = language_nickname_index.find(std::make_tuple("Bob", "Bobby"));
  if (it3 != language_nickname_index.end()) {
    std::cout << "Found: " << (*it3)->name << std::endl;
  } else {
    std::cout << "Not found" << std::endl;
  }

  // Query by age and by_language_nickname
  auto& age_nickname_index = persons.get<by_age_nickname>();
  auto it4 = age_nickname_index.find(std::make_tuple(30, "Bobby"));
  if (it4 != age_nickname_index.end()) {
    std::cout << "Found: " << (*it4)->name << std::endl;
  } else {
    std::cout << "Not found" << std::endl;
  }


  return 0;
}
