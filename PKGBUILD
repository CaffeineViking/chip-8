# Maintainer: Erik S. V. Jansson <caffeineviking@gmail.com>

pkgname=chip-8-git
pkgver=VERSION
pkgrel=1
pkgdesc="Another Chip-8 Interpreter"
arch=('i686' 'x86_64')
url="https://github.com/CaffeineViking/chip-8"
license=('MIT')
provides=('chip-i')
depends=('sdl2' 'sdl2_image')
makedepends=('git')
source=('git+https://github.com/CaffeineViking/chip-i')
md5sums=('SKIP')

pkgver() {
	cd chip-8
	printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

build() {
	cd chip-8
	make
}

package() {
	cd chip-i
	install -Dm755 bin/chip-8 "$pkgdir"/usr/bin/chip-8
	install -Dm644 LICENSE.md "$pkgdir"/usr/share/licenses/$pkgname/LICENSE
}
