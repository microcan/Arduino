#ifndef MICROCAN_COMMON_H
#define MICROCAN_COMMON_H

int limit(int value, int minimum, int maximum)
{
  return min(max(value, minimum), maximum);
}

#endif