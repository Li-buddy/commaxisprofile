#pragma once
/// @file  canopen_scaling_strategy_control.h
///
/// This header file is part of the axisprofile library and define the
/// scaling calculation. 
///
/// @copyright (C) 2018-2022 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.  

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "profiles/scaling/scaling_strategy.h"
#include "profiles/format/pos_format_helper_mod.h"
#include "profiles/format/pos_format_helper_abs.h"
// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------
class TestCanopenScalingStrategyControl;

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
  {
    namespace canopen
    {
      // ---------------------------------------------------------------------------
      // Forward references
      // ---------------------------------------------------------------------------

      // ---------------------------------------------------------------------------
      // Declaration of symbolic constants
      // ---------------------------------------------------------------------------

      // ---------------------------------------------------------------------------
      // Declaration of macros
      // ---------------------------------------------------------------------------

      // ---------------------------------------------------------------------------
      // Declaration of data types
      // ---------------------------------------------------------------------------

      // ---------------------------------------------------------------------------
      // Declaration of classes and structures
      // ---------------------------------------------------------------------------

      /// @brief This is specific class to implement third party CoE scaling method
      class ScalingStrategyCoEControl : public ScalingStrategy
      {
      public: // method

        explicit ScalingStrategyCoEControl(ProfileBasicInfo& profileBasicInfo);
        ScalingStrategyCoEControl() = delete;
        ~ScalingStrategyCoEControl() noexcept override = default;

        AxisProfileResult calcScalingLSBWeights() final;
        AxisProfileResult setScalingNumerator(const std::uint32_t scalingNumerator) final;
        AxisProfileResult setScalingDenominator(const std::uint32_t scalingDenominator) final;
        AxisProfileResult setEncoderResolution(const std::uint32_t resolution) final;
        AxisProfileResult setDeviceScalingType(const AxisProfileDeviceScalingType deviceScalingType) final;
        AxisProfileResult setVelocityFactorCfg(const ScalingFactor& strategyCfg) final;
        AxisProfileResult checkAndSetModulo(double modValue, std::string& dynErrTxt) final;
        double convertPositionRawToScaled(std::int32_t rawTelegramValue) const final;
        std::int32_t convertPositionScaledToRaw(double scaledInternalValue) const final;
        void initModuloPosCmp(std::int32_t rawActPos) final;

      private:// method

        AxisProfileResult calcPosScalingLSBWeight() final;
        AxisProfileResult calcVelScalingLSBWeight() final;
        AxisProfileResult calcAccScalingLSBWeight() final;
        AxisProfileResult calcTorqueScalingLSBWeight() final;

        /// @brief This function is used to calculate the general calculation part of scaling LBS
        /// @param[in] scalingLSBWeight  can be used to enter position scaling LBS weight or velocity scaling LBS weight
        /// @return  S_OK, if success
        AxisProfileResult scalingLBSGeneralCalc(double& scalingLSBWeight);

        /// @brief This function is used to calculate the special scaling factor part.
        /// @param[in, out] scalingLSBWeight for now it can be used for velocity scaling
        /// @return S_OK, if success
        AxisProfileResult calcStandaloneScalingFactor(double& scalingLSBWeight);

      private: // data
        friend class ::TestCanopenScalingStrategyControl;
        mutable format::PosFormatHelperMod m_modulo {};                         ///< The tool object for modulo axis to process data from AT or to MDT
        mutable format::PosFormatHelperAbs m_absolute {};                       ///< The tool object for absolute axis to process data from AT or to MDT
      };
    }
  }
}
