package cb_test

import (
	"testing"

	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/fields/test"
)

// sliding-one

func TestCbFields(t *testing.T) {
	p2m.SettingsReset()
	p2m.Config.Field = p2m.CbFlds
	referenceSlice := []string{
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO) | PAD_RESET(PLTRST), 0),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO) | PAD_RESET(DEEP), 0),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO) | (1 << 29), 0),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO) | (1 << 28), 0),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO), 0),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO) | PAD_TRIG(OFF), 0),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO) | PAD_TRIG(EDGE_SINGLE), PAD_CFG1_TOL_1V8),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO), 0),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO) | PAD_RX_POL(INVERT), 0),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO), 0),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO), 0),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO) | PAD_IRQ_ROUTE(IOAPIC), 0),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO) | PAD_IRQ_ROUTE(SCI), 0),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO) | PAD_IRQ_ROUTE(SMI), 0),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO) | PAD_IRQ_ROUTE(NMI), PAD_IOSSTATE(HIZCRx1)),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO), PAD_IOSSTATE(Tx1RxDCRx1)),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO), PAD_IOSSTATE(Tx0RxDCRx1)),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO), PAD_IOSSTATE(Tx0RxDCRx0)),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO), PAD_PULL(INVALID)),", // Error   invalid TERM value =  8
		"_PAD_CFG_STRUCT(, PAD_FUNC(NF4), PAD_PULL(DN_20K)),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(NF2), PAD_PULL(DN_5K)),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(NF1), PAD_PULL(INVALID)),", // Error   invalid TERM value =  1
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO) | PAD_BUF(RX_DISABLE), PAD_IOSTERM(ENPD)),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO) | PAD_BUF(TX_DISABLE), PAD_IOSTERM(DISPUPD)),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO), 0),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO), 0),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO), 0),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO), 0),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO), 0),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO), 0),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO) | (1 << 1), 0),",
		"_PAD_CFG_STRUCT(, PAD_FUNC(GPIO) | 1, 0),",
	}
	test.SlidingOneTestSuiteCreate(referenceSlice).Run(t, "SLIDING-ONE-TEST")
}
