/*
 * This file is part of liblcf. Copyright (c) liblcf authors.
 * https://github.com/EasyRPG/liblcf - https://easyrpg.org
 *
 * liblcf is Free/Libre Open Source Software, released under the MIT License.
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#ifndef LCF_SCOPE_GUARD_H
#define LCF_SCOPE_GUARD_H
#include <utility>

namespace lcf {

template <typename F>
class ScopeGuard {
	public:
		ScopeGuard() = default;
		explicit ScopeGuard(const F& f) : _f(f), _active(true) {}
		explicit ScopeGuard(F&& f) : _f(std::move(f)), _active(true) {}

		ScopeGuard(const ScopeGuard&) = delete;
		ScopeGuard& operator=(const ScopeGuard&) = delete;

		ScopeGuard(ScopeGuard&& o)
			: _f(std::move(o._f)), _active(true) { o._active = false; }
		ScopeGuard& operator=(ScopeGuard&&) = delete;

		~ScopeGuard() { Fire(); }

		void Fire() noexcept;
		void Dismiss() noexcept;
		bool IsActive() noexcept;
	private:
		F _f;
		bool _active = false;
};

template <typename F>
inline ScopeGuard<F> makeScopeGuard(F&& f) {
	return ScopeGuard<F>(std::forward<F>(f));
}

template <typename F>
inline void ScopeGuard<F>::Fire() noexcept {
	if (_active) {
		_f();
		Dismiss();
	}
}

template <typename F>
inline void ScopeGuard<F>::Dismiss() noexcept {
	_active = false;
}

template <typename F>
inline bool ScopeGuard<F>::IsActive() noexcept {
	return _active;
}

} //namespace lcf

#endif
