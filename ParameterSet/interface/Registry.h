#ifndef MessageFacility_ParameterSet_Registry_h
#define MessageFacility_ParameterSet_Registry_h

// ----------------------------------------------------------------------
// Declaration for pset::Registry. This is an implementation detail of
// the ParameterSet library.
//
// A Registry is used to keep track of the persistent form of all
// ParameterSets used a given program, so that they may be retrieved by
// ParameterSetID, and so they may be written to persistent storage.
// ----------------------------------------------------------------------

#include <map>

#include "ParameterSet/interface/ParameterSetID.h"
#include "ParameterSet/interface/ParameterSetBlob.h"
#include "ParameterSet/interface/ParameterSet.h"
#include "Utilities/interface/ThreadSafeRegistry.h"



namespace mf {
  namespace pset {

    class ProcessParameterSetIDCache {
    public:
      ProcessParameterSetIDCache() : id_() { }
      ParameterSetID id() const { return id_; }
      void setID(ParameterSetID const& id) { id_ = id; }
    private:
      ParameterSetID id_;      
    };

    typedef detail::ThreadSafeRegistry<ParameterSetID,
    					ParameterSet,
					ProcessParameterSetIDCache>
                                        Registry;

    /// Associated free functions.

    /// Return the ParameterSetID of the top-level ParameterSet stored
    /// in the given Registry. Note the the returned ParameterSetID may
    /// be invalid; this will happen if the Registry has not yet been
    /// filled.
    ParameterSetID getProcessParameterSetID(Registry const* reg);

    /// Fill the given map with the persistent form of each
    /// ParameterSet in the given registry.
    typedef std::map<ParameterSetID, ParameterSetBlob> regmap_type;
    void fillMap(Registry* reg, regmap_type& fillme);

  }  // namespace pset

  ParameterSet getProcessParameterSet();

}  // namespace mf


#endif
