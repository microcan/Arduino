#ifndef MICROCAN_COMMON_H
#define MICROCAN_COMMON_H

// limit an int to a range
int limit(int value, int minimum, int maximum)
{
  return min(max(value, minimum), maximum);
}

// limit a float to a range
float limit(float value, float minimum, float maximum)
{
  return min(max(value, minimum), maximum);
}

// turn a normalized value into a color gradient from blue to green to 
// orange to red.
void colorize(float value, byte& r, byte& g, byte& b)
{
  float norm = limit(value, 0.0, 1.0);
  // blue - 0
  // green - 0.5
  // orange - 0.75
  // red - 1.0
  if (norm <= 0.5)
  {
    float miniNorm = norm * 2.0;
    r = 0;
    g = 255.0 * (miniNorm);
    b = 255.0 * (1.0 - miniNorm);
  }
  else if (norm <= 0.75)
  {
    float miniNorm = (norm - 0.5) * 4.0;
    r = 255.0 * miniNorm;
    g = 255.0 * (1.0 - miniNorm) + 165.0 * (miniNorm);
    b = 0;
  }
  else
  {
    float miniNorm = (norm - 0.75) * 4.0;
    r = 255.0;
    g = 165.0 * (1.0 - miniNorm);
    b = 0;
  }
}

#endif