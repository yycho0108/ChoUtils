#pragma once

#include <memory>

namespace cho {
namespace vis {

class AbstractHandler {
 public:
  using Ptr = std::shared_ptr<AbstractHandler>;
  virtual ~AbstractHandler() = default;

  virtual void Create() = 0;
  virtual void Update() = 0;
};

class Handler;
using HandlerPtr = std::shared_ptr<Handler>;

}  // namespace vis
}  // namespace cho
