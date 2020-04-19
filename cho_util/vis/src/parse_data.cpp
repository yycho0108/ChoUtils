
int main() {
  // Representation
  switch (request_.representation()) {
    case render::RepresentationType::kAsNone: {
      break;
    }
    default: { break; }
  }

  // Color
  switch (request_.color_case()) {
    case RenderRequest::ColorCase::kEach: {
      // Every point gets a color
      break;
    }
    case RenderRequest::ColorCase::kUniform: {
      // All points are same color
      break;
    }
    default: {
      // All points are default color
      break;
    }
  }
}
