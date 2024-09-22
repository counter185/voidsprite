#include "lcf/saveopt.h"
#include "lcf/rpg/database.h"

namespace lcf {

EngineVersion GetEngineVersion(const lcf::rpg::Database& db) {
	if (db.system.ldb_id == 2003) {
		return EngineVersion::e2k3;
	}
	return EngineVersion::e2k;
}

} // namespace lcf
