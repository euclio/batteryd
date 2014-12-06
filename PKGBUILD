# Maintainer: Andy Russell <andy@acrussell.com>

pkgname=batteryd-git
pkgver=
pkgrel=1
pkgdesc="A small battery daemon for Archlinux"
arch=("i686 x86_64")
url=("https://github.com/euclio/batteryd")
license=("GPL3")
depends=("libnotify")
makedepends=("git" "gcc" "cmake" "boost")
source=("git+http://github.com/euclio/batteryd")
md5sums=("SKIP")
install="batteryd.install"

pkgver() {
  cd batteryd
  printf "%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

build() {
  cd ${srcdir}/batteryd
  cmake -DCMAKE_INSTALL_PREFIX=/usr   \
        -DCMAKE_BUILD_TYPE=Release
  make
}

package() {
  install -D -m644 "${srcdir}/batteryd/batteryd@.service" \
    "${pkgdir}/usr/lib/systemd/system/batteryd@.service"

  install -D -m644 "${srcdir}/batteryd/batteryd.rules" \
    "${pkgdir}/usr/share/polkit-1/rules.d/batteryd.rules"

  install -D -m644 "${srcdir}/batteryd/LICENSE" \
    "${pkgdir}/usr/share/licenses/batteryd/LICENSE"

  cd ${srcdir}/batteryd
  make DESTDIR="$pkgdir" install
}
md5sums=('SKIP')
