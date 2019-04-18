
# Maintainer: Steven Mattera <me@stevenmattera.com>
pkgname=switch-swurl
pkgver=1.0.0
pkgrel=1
pkgdesc='A simple wrapper library for CURL on the Nintendo Switch.'
arch=('any')
license=('custom')
url='https://github.com/AtlasNX/Swurl'
options=(!strip libtool staticlibs)
depends=('switch-curl' 'switch-zlib' 'switch-mbedtls')
makedepends=('switch-pkg-config' 'devkitpro-pkgbuild-helpers')
source=("https://github.com/AtlasNX/Swurl/archive/v${pkgver}.tar.gz")

sha256sums=('c93c3b90e3ca620785134181805a6534f6d2afdaff2a96fb86efc93fa7108b05')

build() {
  cd Swurl-$pkgver

  source /opt/devkitpro/switchvars.sh

  make  
}

package() {
  cd Swurl-$pkgver

  source /opt/devkitpro/switchvars.sh

  make DESTDIR="$pkgdir" install
  rm $pkgdir$PORTLIBS_PREFIX/LICENSE
}
