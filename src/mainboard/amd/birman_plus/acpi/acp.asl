/* SPDX-License-Identifier: GPL-2.0-only */

/* ACP Audio Configuration */
Scope (\_SB.PCI0.GP41.ACPD) {

	/* Global Configuration - _DSD properties for ACP device */
	Name (_DSD, Package (0x02)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x02)
		{
			Package (0x02) {"acp-audio-zsc-enable", 0x01},
			Package (0x02) {"acp-audio-config-flag", 0x10}
		}
	})
}
Scope (\_SB.PCI0.GP41.ACPD.HDA0) {
	Name (_DSD, Package ()
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package (2) {"acp-audio-device-interface-version",0x01},
			Package (2) {"acp-audio-device-type",0x01}
		},

		ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
		Package ()
		{
			Package (0x02) {"acp-audio-device-eps",
			Package () { "PE00","PE01"}
			}
		}
	})
	Name (PE00, Package ()
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package (0x02) {"acp-audio-ep-type", 0x0 },
			Package (0x02) {"acp-audio-ep-dsp-offload-supported",0x1},
			Package (0x02) {"acp-audio-ep-category", 0x1}
		},

		ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
		Package ()
		{
			Package (0x02){"acp-audio-ep-format", "FMTS"},
			Package (0x02){"acp-audio-ep-apo-fx-type-ex", "EFXS"}
		}
	})
	Name (PE01, Package ()
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package (0x02){"acp-audio-ep-type", 0x0},
			Package (0x02){"acp-audio-ep-dsp-offload-supported", 0x1},
			Package (0x02){"acp-audio-ep-category", 0x2}
		},

		ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
		Package ()
		{
			Package (0x02){"acp-audio-ep-format", "FMTH"},
			Package (0x02){"acp-audio-ep-apo-fx-type-ex","EFXH"}
		}
	})
	Name (FMTS, Package ()
		{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package (0x02){"acp-audio-ep-format-max-channels", 0x02},
			Package (0x02){"acp-audio-ep-format-sampling-frequency",
			Package (){ 48000,96000}
			},
			Package (0x02){"acp-audio-ep-format-bits-per-sample",
			Package (){16,24}
			}
		}
	})
	Name (FMTH, Package ()
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package (0x02){"acp-audio-ep-format-max-channels", 0x02},
			Package (0x02){"acp-audio-ep-format-sampling-frequency",
			Package (){ 48000,96000}
			},
			Package (0x02){"acp-audio-ep-format-bits-per-sample",
			Package (){16,24}
			}
		}
	})
	Name (EFXS, Package ()
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package (0x02){"{05714B44-A61C-40F0-9F72-3A2EFF40D74C}", "{00000000-0000-0000-0000-000000000000}"},
			Package (0x02){"{591689D6-7499-4444-BFDE-BA2BE1D9A5D6}", "{00000000-0000-0000-0000-000000000000}"},
			Package (0x02){"{E5E0FB95-F997-4EB4-9580-35A81438FE5B}", "{00000000-0000-0000-0000-000000000000}"}
		}
		})
		Name (EFXH, Package ()
		{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package (0x02){"{591689D6-7499-4444-BFDE-BA2BE1D9A5D6}", "{00000000-0000-0000-0000-000000000000}"},
			Package (0x02){"{E5E0FB95-F997-4EB4-9580-35A81438FE5B}", "{00000000-0000-0000-0000-000000000000}"}
		}
	})
}
Scope (\_SB.PCI0.GP41.ACPD.BTSC) {
	Name (_DSD, Package ()
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package (2) {"acp-audio-device-interface-version",0x01},
			Package (2) {"acp-audio-device-type",0x04}
		},

		ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
		Package ()
		{
			Package (0x02) {"acp-audio-device-eps",
			Package () { "PE00","CE00"}
			}
		}
	})
	Name (PE00, Package ()
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package (0x02) {"acp-audio-ep-type", 0x0 },
			Package (0x02) {"acp-audio-ep-dsp-offload-supported",0x1},
			Package (0x02) {"acp-audio-ep-category", 0x1},
			Package (0x02) {"acp-audio-ep-port", 0x3},
			Package (0x02) {"acp-audio-ep-node", 0x2},
			Package (0x02) {"acp-audio-ep-capabilities",
			Package (){ 0x1, 0x2, 0x4, 0x8}}
		},

		ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
		Package ()
		{
			Package (0x02){"acp-audio-ep-format", "FMT0"},
			Package (0x02){"acp-audio-ep-apo-fx-type", "EFX0"},
			Package (0x02){"acp-audio-ep-apo-fx-type-ex","EFX1"}
		}
	})
	Name (FMT0, Package ()
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package (0x02){"acp-audio-ep-format-max-channels", 0x02},
			Package (0x02){"acp-audio-ep-format-sampling-frequency",
			Package (){ 8000,16000, 48000}},
			Package (0x02){"acp-audio-ep-format-bits-per-sample",
			Package (){16}}
		}
	})
	Name (EFX0, Package ()
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package (0x02){"acp-audio-ep-apo-efx-type", 0x0},
			Package (0x02){"acp-audio-ep-apo-mfx-type", 0x0},
			Package (0x02){"acp-audio-ep-apo-sfx-type", 0x0},
			Package (0x02){"acp-audio-ep-apo-efx-encoder-type", 0x100}
		}
	})
	Name (EFX1, Package ()
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package (0x02){"{591689D6-7499-4444-BFDE-BA2BE1D9A5D6}", "{00000000-0000-0000-0000-000000000000}"},
			Package (0x02){"{E5E0FB95-F997-4EB4-9580-35A81438FE5B}", "{00000000-0000-0000-0000-000000000000}"},
			Package (0x02){"{09DF37AE-0217-4601-A842-DEC90EEBC68F}", "{66E5A40D-4559-4019-B0F1-5F842C7E461A}"},
		}
	})
	Name (CE00, Package ()
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package (0x02) {"acp-audio-ep-type", 0x1 },
			Package (0x02) {"acp-audio-ep-dsp-offload-supported",0x1},
			Package (0x02) {"acp-audio-ep-ai-noise-reduction-supported", 0x0},
			Package (0x02) {"acp-audio-ep-port", 0x3},
			Package (0x02) {"acp-audio-ep-node", 0x2}
		},

		ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
		Package ()
		{
			Package (0x02){"acp-audio-ep-format", "FMT1"},
			Package (0x02){"acp-audio-ep-apo-fx-type", "EFX2"},
			Package (0x02){"acp-audio-ep-apo-fx-type-ex","EFX3"}
		}
	})
	Name (FMT1, Package ()
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package (0x02){"acp-audio-ep-format-max-channels", 0x02},
			Package (0x02){"acp-audio-ep-format-sampling-frequency",
			Package (){ 8000,16000}},
			Package (0x02){"acp-audio-ep-format-bits-per-sample",
			Package (){16}}
		}
	})
	Name (EFX2, Package ()
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package (0x02){"acp-audio-ep-apo-efx-type", 0x0},
			Package (0x02){"acp-audio-ep-apo-mfx-type", 0x0},
			Package (0x02){"acp-audio-ep-apo-sfx-type", 0x0},
			Package (0x02){"acp-audio-ep-apo-efx-encoder-type", 0x100}
		}
	})
	Name (EFX3, Package ()
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package (0x02){"{65E94A28-DE71-4154-AD49-F67C7831EA4D}", "{00000000-0000-0000-0000-000000000000}"},
			Package (0x02){"{E5B78E46-DAA2-4827-8D84-F6C0C0306541}", "{00000000-0000-0000-0000-000000000000}"},
			Package (0x02){"{09DF37AE-0217-4601-A842-DEC90EEBC68F}", "{66E5A40D-4559-4019-B0F1-5F842C7E461A}"}
		}
	})
}
