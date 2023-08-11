pkgname=rooibos
pkgver=0.1.0
pkgrel=0
pkgdesc="A modern window manager for X11"
arch=('any')
url="https://github.com/FrancoisBasset/rooibos"
license=('MIT')
install=install
depends=('gdk-pixbuf2' 'alsa-lib' 'xterm' 'xfe')

build() {
	cd ..
	make
}

package() {
	cd ..
	make pkgs
	rm -r src
	cp -r package/* pkg/rooibos
}