#include <core.hpp>

ACTION core::hi(name user) {
  require_auth(user);
  print("Hello, ", name{user});
}
