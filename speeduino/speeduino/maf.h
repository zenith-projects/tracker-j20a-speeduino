#pragma once

#include <stdint.h>

/** Synthetic manifold pressure from a measured mass-airflow.
 *
 * Speeduino fuel pulse is reqFuel * VE * (load / 100). reqFuel is the pulse
 * for one cylinder of air at 100 kPa, 20 C and 100% VE. Dividing the measured
 * air mass per intake by that reference mass produces a kPa the existing
 * equation already understands. VE stays a trim: 100 trusts the MAF curve.
 *
 * flowTenthsGps is grams/second * 10. refMg is milligrams of air in one
 * cylinder at 100 kPa, 20 C and 100% VE (cylinder cc * 1.189).
 * A 1995 cc four-cylinder is 593 mg.
 */
static inline uint16_t mafSyntheticKpa(uint16_t flowTenthsGps, uint16_t rpm, uint8_t cylinders, uint8_t twoStroke, uint16_t refMg)
{
  if ((rpm == 0U) || (flowTenthsGps == 0U))
  {
    return 0U;
  }

  uint8_t cyl = (cylinders == 0U) ? 1U : cylinders;
  uint16_t ref = (refMg < 50U) ? 593U : refMg;
  uint16_t flow = (flowTenthsGps > 3000U) ? 3000U : flowTenthsGps;
  // Four-stroke: g/cyl = (g/s) * 120 / (rpm * cyl). The 1_200_000 absorbs the 0.1 g/s scale and the *100 for kPa.
  uint32_t perCycle = (twoStroke != 0U) ? 600000UL : 1200000UL;
  uint32_t kpa = ((uint32_t)flow * perCycle) / ((uint32_t)rpm * cyl * ref);
  if (kpa > 511UL)
  {
    kpa = 511UL;
  }
  return (uint16_t)kpa;
}
