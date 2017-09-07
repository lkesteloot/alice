
# sudo apt-get install device-tree-compiler

.PHONY: clean install

INSTALL_DIR=/mnt/mmc

new.dtb: source.dts
	dtc --in-format dts --out new.dtb --out-format dtb source.dts

source.dts: orig.dtb
	dtc --in-format dtb --out source.dts --out-format dts orig.dtb

clean:
	rm -f new.dtb source.dts

install: orig.dtb new.dtb
	sudo cp orig.dtb $(INSTALL_DIR)
	sudo cp new.dtb $(INSTALL_DIR)/zImage-socfpga_cyclone5_de0_sockit.dtb

