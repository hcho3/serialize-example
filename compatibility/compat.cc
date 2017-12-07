#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>
#include <string>
#include <iostream>
#include <fstream>

class Version1 {
 private:
  int cat;
  double dog;

  template<class Archive>
  void serialize(Archive& archive, const uint32_t version) {
    if (version > 100) {
      std::cerr << "Warning: reading from a future version; "
                << "unsupported fields have been ignored." << std::endl;
    }
    archive(CEREAL_NVP(cat), CEREAL_NVP(dog));
  }

  friend std::ostream& operator<<(std::ostream &os, const Version1& o);
  friend cereal::access;

 public:
  Version1() = default;
  Version1(int cat, double dog) : cat(cat), dog(dog) {}
};

std::ostream& operator<<(std::ostream &os, const Version1& o) { 
  return os << "{cat=" << o.cat << ", dog=" << o.dog << "}";
}

class Version2 {
 private:
  int cat;
  double dog;
  std::string foo;

  template<class Archive>
  void serialize(Archive& archive, const uint32_t version) {
    if (version > 100) {
      archive(CEREAL_NVP(cat), CEREAL_NVP(dog), CEREAL_NVP(foo));
    } else {
      archive(CEREAL_NVP(cat), CEREAL_NVP(dog));
      std::cerr << "Warning: reading from a previous version; "
                << "foo not initialized" << std::endl;
    }
  }

  friend std::ostream& operator<<(std::ostream &os, const Version2& o);
  friend cereal::access;

 public:
  Version2() : foo("default") {}
  Version2(int cat, double dog, const std::string& foo)
    : cat(cat), dog(dog), foo(foo) {}
};

std::ostream& operator<<(std::ostream &os, const Version2& o) { 
  return os << "{cat=" << o.cat << ", dog=" << o.dog
            << ", foo=\"" << o.foo << "\"}";
}

CEREAL_CLASS_VERSION(Version1, 100);
CEREAL_CLASS_VERSION(Version2, 110);

int main(void) {
#ifdef USE_BINARY
  using MyInputArchive = cereal::PortableBinaryInputArchive;
  using MyOutputArchive = cereal::PortableBinaryOutputArchive;
  #define FILE_HANDLE(x) x ".bin", std::ios::binary
#else
  using MyInputArchive = cereal::JSONInputArchive;
  using MyOutputArchive = cereal::JSONOutputArchive;
  #define FILE_HANDLE(x) x ".json"
#endif

  /* self compatibility */
  {
    {
      std::ofstream fout(FILE_HANDLE("test"));
      MyOutputArchive archive(fout);
      Version1 v1(400, -0.743);
      archive(v1);
    }
    {
      std::ifstream fin(FILE_HANDLE("test"));
      MyInputArchive archive(fin);
      Version1 v1;
      archive(v1);
      std::cout << "v1 = " << v1 << std::endl;
    }
    std::cout << std::endl;
    {
      std::ofstream fout(FILE_HANDLE("test2"));
      MyOutputArchive archive(fout);
      Version2 v2(400, -0.743, "foo");
      archive(v2);
    }
    {
      std::ifstream fin(FILE_HANDLE("test2"));
      MyInputArchive archive(fin);
      Version2 v2;
      archive(v2);
      std::cout << "v2 = " << v2 << std::endl;
    }
  }
  std::cout << std::endl;
  /* forward compatibility */
  {
    {
      std::ofstream fout(FILE_HANDLE("test3"));
      MyOutputArchive archive(fout);
      Version1 v1(400, -0.743);
      archive(v1);
    }
    {
      std::ifstream fin(FILE_HANDLE("test3"));
      MyInputArchive archive(fin);
      Version2 v2;
      archive(v2);
      std::cout << "v2 = " << v2 << std::endl;
    }
  }
  std::cout << std::endl;
  /* backward compatibility */
  {
    {
      std::ofstream fout(FILE_HANDLE("test4"));
      MyOutputArchive archive(fout);
      Version2 v2(400, -0.743, "foo");
      archive(v2);
    }
    {
      std::ifstream fin(FILE_HANDLE("test4"));
      MyInputArchive archive(fin);
      Version1 v1;
      archive(v1);
      std::cout << "v1 = " << v1 << std::endl;
    }
  }
  std::cout << std::endl;
}