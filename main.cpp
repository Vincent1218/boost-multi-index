// This is a test of C++ Boost library multi-index container

#include <cstdint>
#include <iostream>
#include <string>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/key.hpp>


// Custom string with template to pass in length
template <std::size_t N>
struct custom_string {
  char data[N];

  // get data
  const char* c_str() const {
    return data;
  }

  // hash
  std::size_t hash() const {
    return std::hash<std::string_view>{}(data);
  }

  // comparison
  bool operator==(const custom_string& other) const {
    return std::string_view(data) == std::string_view(other.data);
  }

  bool operator<(const custom_string& other) const {
    return std::string_view(data) < std::string_view(other.data);
  }

  bool operator>(const custom_string& other) const {
    return std::string_view(data) > std::string_view(other.data);
  }

  bool operator<=(const custom_string& other) const {
    return std::string_view(data) <= std::string_view(other.data);
  }

  bool operator>=(const custom_string& other) const {
    return std::string_view(data) >= std::string_view(other.data);
  }

  bool operator!=(const custom_string& other) const {
    return std::string_view(data) != std::string_view(other.data);
  }

  // stream operator
  friend std::ostream& operator<<(std::ostream& os, const custom_string& s) {
    os << s.data;
    return os;
  }
  
  // copy assignment
  custom_string& operator=(const char* str) {
    std::strncpy(data, str, sizeof(data));
    return *this;
  }

  // copy assignment
  custom_string& operator=(const custom_string& other) {
    std::strncpy(data, other.data, sizeof(data));
    return *this;
  }

  // copy constructor
  custom_string(const custom_string& other) {
    std::strncpy(data, other.data, sizeof(data)); 
  }

  // copy constructor
  custom_string(const char* str) {
    std::strncpy(data, str, sizeof(data));
  }

  // default constructor
  custom_string() {
    std::memset(data, 0, sizeof(data));
  }

  // destructor
  ~custom_string() {
    std::memset(data, 0, sizeof(data));
  }


};

// define custom hash function in std for custom_string
namespace std {
  template <std::size_t N>
  struct hash<custom_string<N>> {
    std::size_t operator()(const custom_string<N>& s) const {
      return s.hash();
    }
  };
}

struct PersonString : custom_string<32> {};

int main (int argc, char *argv[]) {
  // a mock class for data Person
  struct Person {
    uint16_t id;
    PersonString name;
    uint16_t age;
    PersonString nickname;
    PersonString language;
  };

  // custom hash function for PersonString
  struct PersonStringHash {
    std::size_t operator()(const PersonString& s) const {
      return s.hash();
    }
  };

  // Define a multi-index container 

  // tag struct for accessing the index
  struct by_id {};
  struct by_name {};
  struct by_language_nickname {};
  struct by_age_nickname {};
  struct by_id_name {};

  typedef boost::multi_index_container<
    Person*,
    boost::multi_index::indexed_by<
      boost::multi_index::hashed_unique<
        boost::multi_index::tag<by_name>,
        boost::multi_index::key<&Person::name>,
        PersonStringHash
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
          PersonStringHash,
          PersonStringHash
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
          PersonStringHash
        >
      >,
      boost::multi_index::hashed_unique<
        boost::multi_index::tag<by_id_name>,
        boost::multi_index::composite_key<
          Person,
          boost::multi_index::key<&Person::id>,
          boost::multi_index::key<&Person::name>
        >,
        boost::multi_index::composite_key_hash<
          std::hash<int>,
          PersonStringHash
        >
      >
    > 
  > PersonMultiIndexContainer;

  // vector of unique pointers
  std::vector<std::unique_ptr<Person>> persons_vec;
  persons_vec.push_back(std::make_unique<Person>(Person{1, "Alice", 20, "Ali", "English"}));
  persons_vec.push_back(std::make_unique<Person>(Person{2, "Bob", 30, "Bobby", "French"}));
  persons_vec.push_back(std::make_unique<Person>(Person{3, "Cathy", 40, "Cat", "Spanish"}));    
  persons_vec.push_back(std::make_unique<Person>(Person{4, "David", 50, "Dave", "German"}));
  persons_vec.push_back(std::make_unique<Person>(Person{5, "Eva", 60, "Eve", "Italian"}));
  persons_vec.push_back(std::make_unique<Person>(Person{6, "Frank", 70, "Frankie", "Chinese"}));
  persons_vec.push_back(std::make_unique<Person>(Person{7, "Grace", 80, "Gracie", "Japanese"}));
  persons_vec.push_back(std::make_unique<Person>(Person{8, "Hank", 90, "Hankie", "Korean"}));
  persons_vec.push_back(std::make_unique<Person>(Person{9, "Ivy", 100, "Ive", "Russian"}));
  persons_vec.push_back(std::make_unique<Person>(Person{10, "YTS2", 110, "Jackie", "Arabic"}));

  PersonMultiIndexContainer persons;
  for (auto& p : persons_vec) {
    persons.insert(p.get());
  }


  int count = 0;
  // Query data from the container
  
  // Query by name
  auto& name_index = persons.get<by_name>();
  for (auto& p : persons_vec) {
    auto it = name_index.find(p->name);
    if (it != name_index.end()) {
      std::cout << "Found: " << (*it)->name << std::endl;
      count++;
    } else {
      std::cout << "Not found" << std::endl;
    }
  }
  std::cout << "Count: " << count << std::endl;
  std::cout << std::endl;
  

  // Query by by_id
  auto& id_index = persons.get<by_id>();
  for (auto& p : persons_vec) {
    auto it = id_index.find(p->id);
    if (it != id_index.end()) {
      std::cout << "Found: " << (*it)->name << std::endl;
      count++;
    } else {
      std::cout << "Not found" << std::endl;
    }
  }
  std::cout << "Count: " << count << std::endl;
  std::cout << std::endl;

  // Query by language and by_language_nickname
  auto& language_nickname_index = persons.get<by_language_nickname>();
  for (auto& p : persons_vec) {
    auto it = language_nickname_index.find(std::make_tuple(p->name, p->nickname));
    if (it != language_nickname_index.end()) {
      std::cout << "Found: " << (*it)->name << std::endl;
      count++;
    } else {
      std::cout << "Not found" << std::endl;
    }
  }
  std::cout << "Count: " << count << std::endl;
  std::cout << std::endl;

  // Query by age and by_language_nickname
  auto& age_nickname_index = persons.get<by_age_nickname>();
  for (auto& p : persons_vec) {
    auto it = age_nickname_index.find(std::make_tuple(p->age, p->nickname));
    if (it != age_nickname_index.end()) {
      std::cout << "Found: " << (*it)->name << std::endl;
      count++;
    } else {
      std::cout << "Not found" << std::endl;
    }
  }
  std::cout << "Count: " << count << std::endl;
  std::cout << std::endl;

  // Query by id and by_id_name
  auto& id_name_index = persons.get<by_id_name>();
  for (auto& p : persons_vec) {
    auto it = id_name_index.find(std::make_tuple(p->id, p->name));
    if (it != id_name_index.end()) {
      std::cout << "Found: " << (*it)->name << std::endl;
      count++;
    } else {
      std::cout << "Not found" << std::endl;
    }
  }
  std::cout << "Count: " << count << std::endl;
  std::cout << std::endl;

  return 0;
}
