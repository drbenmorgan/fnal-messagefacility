#ifndef messagefacility_Utilities_EnabledState_h
#define messagefacility_Utilities_EnabledState_h
////////////////////////////////////////////////////////////////////////
// EnabledState
//
// Users should just pass this between mf::saveEnabledState() and
// mf::restoreEnabledState() as appropriate.
//
////////////////////////////////////////////////////////////////////////
namespace mf {
  class EnabledState;
}

// For saving and restoring state.
class mf::EnabledState {
public:
  EnabledState() = default;
  EnabledState(bool const d, bool const i, bool const w);

  void
  reset()
  {
    isValid_ = false;
  }

  bool
  isValid() const
  {
    return isValid_;
  }
  bool
  debugEnabled() const
  {
    return debugEnabled_;
  }
  bool
  infoEnabled() const
  {
    return infoEnabled_;
  }
  bool
  warningEnabled() const
  {
    return warningEnabled_;
  }

private:
  bool debugEnabled_{false};
  bool infoEnabled_{false};
  bool warningEnabled_{false};
  bool isValid_{false};
};

inline mf::EnabledState::EnabledState(bool const d, bool const i, bool const w)
  : debugEnabled_{d}, infoEnabled_{i}, warningEnabled_{w}, isValid_{true}
{}

#endif /* messagefacility_Utilities_EnabledState_h */

// Local Variables:
// mode: c++
// End:
