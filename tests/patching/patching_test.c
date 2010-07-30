#include <bss_patch.h>

#include <laf0rge1/debug.h>

#include <stdio.h>
#include <string.h>

uint8_t pkt125[] = {
0x06, 0x01, 0x04, 
0x00, 0x00, 0x01, 0x0b, 0x00, 0x09, 0x01, 0x0b, 
0x03, 0x01, 0x0b, 0x25, 0x01, 0x00, 0x02 };

uint8_t pkt125_res[] = {
0x06, 0x01, 0x04, 
0x00, 0x00, 0x01, 0x0b, 0x00, 0x09, 0x01, 0x0b, 
0x03, 0x01, 0x0a, 0x11, 0x01, 0x00, 0x02 };

uint8_t pkt28[] = {
0x06, 0x01, 0x05,
0x2b, 0x00, 0x01, 0x09, 0x00, 0x07, 0x02, 0x21,
0x08, 0x2c, 0x02, 0x40, 0x11 };
uint8_t pkt28_res[] = {
0x06, 0x01, 0x05,
0x2b, 0x00, 0x01, 0x09, 0x00, 0x07, 0x02, 0x21,
0x09, 0x2c, 0x02, 0x40, 0x25 };

uint8_t reset_ack[] = {
0x09, 0x00, 0x03,
0x07, 0x0b, 0x04, 0x43, 0x0a, 0x00, 0xfe, 0x04,
0x43, 0x5c, 0x00, 0xfe, 0x03, 0x00, 0x01, 0x31 };

uint8_t cc[] = {
0x02, 0x01, 0x04, 
0x00, 0x01, 0x01, 0xb4, 0x02, 0x01, 0x00 };


struct result {
	uint8_t *input;
	int	  inp_len;
	const uint8_t *expected;
	int       exp_len;
	int	  result;
};


static struct result results[] = {
	{
		.input = pkt125,
		.inp_len = sizeof(pkt125),
		.expected = pkt125_res,
		.exp_len = sizeof(pkt125),
	},

	{
		.input = pkt28,
		.inp_len = sizeof(pkt28),
		.expected = pkt28_res,
		.exp_len = sizeof(pkt28_res),
	},

	{
		.input = reset_ack,
		.inp_len = sizeof(reset_ack),
		.expected = reset_ack,
		.exp_len = sizeof(reset_ack),
		.result = BSS_FILTER_RESET_ACK,
	},

	{
		.input = cc,
		.inp_len = sizeof(cc),
		.expected = cc,
		.exp_len = sizeof(cc),
		.result = 0,
	},
};

static uint8_t udt_with_poi[] = {
0x09, 0x00, 0x03,
0x07, 0x0b, 0x04, 0x43, 0x0a, 0x00, 0xfe, 0x04,
0x43, 0x5c, 0x00, 0xfe, 0x10, 0x00, 0x0e, 0x44,
0x04, 0x01, 0x00, 0x01, 0x00, 0x01, 0x1e, 0x05,
0x1e, 0x00, 0x00, 0x00, 0x40 };

static const uint8_t udt_without_poi[] = {
0x09, 0x00, 0x03,
0x05, 0x07, 0x02, 0x42, 0xfe,
            0x02, 0x42, 0xfe,
0x10, 0x00, 0x0e, 0x44,
0x04, 0x01, 0x00, 0x01, 0x00, 0x01, 0x1e, 0x05,
0x1e, 0x00, 0x00, 0x00, 0x40 };

static uint8_t cr_with_poi [] = {
0x01, 0x01, 0x04,
0x00, 0x02, 0x02, 0x06, 0x04, 0xc3, 0x5c, 0x00,
0xfe, 0x0f, 0x21, 0x00, 0x1f, 0x57, 0x05, 0x08,
0x00, 0x72, 0xf4, 0x80, 0x23, 0x29, 0xc3, 0x50,
0x17, 0x10, 0x05, 0x24, 0x11, 0x03, 0x33, 0x19,
0x81, 0x08, 0x29, 0x47, 0x80, 0x00, 0x00, 0x00,
0x00, 0x80, 0x21, 0x01, 0x00 };

static const uint8_t cr_without_poi [] = {
0x01, 0x01, 0x04,
0x00, 0x02, 0x02, 0x04, 0x02, 0x42, 0xfe,
0x0f, 0x21, 0x00, 0x1f, 0x57, 0x05, 0x08,
0x00, 0x72, 0xf4, 0x80, 0x23, 0x29, 0xc3, 0x50,
0x17, 0x10, 0x05, 0x24, 0x11, 0x03, 0x33, 0x19,
0x81, 0x08, 0x29, 0x47, 0x80, 0x00, 0x00, 0x00,
0x00, 0x80, 0x21, 0x01, 0x00 };

static uint8_t cr2_without_poi[] = {
0x01, 0x00, 0x00, 0x03, 0x02, 0x02, 0x04, 0x02,
0x42, 0xfe, 0x0f, 0x1f, 0x00, 0x1d, 0x57, 0x05,
0x08, 0x00, 0x72, 0xf4, 0x80, 0x20, 0x1d, 0xc3,
0x50, 0x17, 0x10, 0x05, 0x24, 0x31, 0x03, 0x50,
0x18, 0x93, 0x08, 0x29, 0x47, 0x80, 0x00, 0x00,
0x00, 0x00, 0x80, 0x00};

static struct result rewrite_results_to_msc[] = {
	{
		.input = udt_with_poi,
		.inp_len = sizeof(udt_with_poi),
		.expected = udt_without_poi,
		.exp_len = sizeof(udt_without_poi),
	},

	{
		.input = cr_with_poi,
		.inp_len = sizeof(cr_with_poi),
		.expected = cr_without_poi,
		.exp_len = sizeof(cr_without_poi),
	},

	{
		.input = cr2_without_poi,
		.inp_len = sizeof(cr2_without_poi),
		.expected = cr2_without_poi,
		.exp_len = sizeof(cr2_without_poi),
	},
};


uint8_t paging_cmd[] = {
	0x09, 0x00, 0x03,  0x07, 0x0b, 0x04, 0x43, 0x0a,
	0x00, 0xfe, 0x04,  0x43, 0x5c, 0x00, 0xfe, 0x10,
	0x00, 0x0e, 0x52,  0x08, 0x08, 0x29, 0x80, 0x10,
	0x76, 0x10, 0x77,  0x46, 0x05, 0x1a, 0x01, 0x06 };
uint8_t paging_cmd_patched[] = {
	0x09, 0x00, 0x03,  0x07, 0x0b, 0x04, 0x43, 0x02,
	0x00, 0xfe, 0x04,  0x43, 0x01, 0x00, 0xfe, 0x10,
	0x00, 0x0e, 0x52,  0x08, 0x08, 0x29, 0x80, 0x10,
	0x76, 0x10, 0x77,  0x46, 0x05, 0x1a, 0x01, 0x06 };

static struct result rewrite_results_to_bsc[] = {
	{
		.input = paging_cmd,
		.inp_len = sizeof(paging_cmd),
		.expected = paging_cmd_patched,
		.exp_len = sizeof(paging_cmd_patched),
	},
};

static void test_patch_filter(void)
{
	int i;

	printf("Testing patching of GSM messages to the MSC.\n");

	for (i = 0; i < sizeof(results)/sizeof(results[0]); ++i) {
		struct sccp_parse_result result;
		struct msgb *msg;
		int rc;

		msg = msgb_alloc_headroom(256, 8, "test");
		msgb_put(msg, 1);
		msg->l2h = msgb_put(msg, results[i].inp_len);
		memcpy(msg->l2h, results[i].input, msgb_l2len(msg));
		rc = bss_patch_filter_msg(msg, &result);

		if (memcmp(msg->l2h, results[i].expected, results[i].exp_len) != 0) {
			printf("Failed to patch the packet.\n");
			abort();
		}
		if (rc != results[i].result) {
			printf("Didn't get the result we wanted on %d\n", i),
			abort();
		}

		msgb_free(msg);
	}
}

static void test_rewrite_msc(void)
{
	int i;

	printf("Testing rewriting the SCCP header.\n");
	for (i = 0; i < sizeof(rewrite_results_to_msc)/sizeof(rewrite_results_to_msc[0]); ++i) {
		struct sccp_parse_result result;
		struct msgb *inp;
		struct msgb *outp;
		int rc;

		outp = msgb_alloc_headroom(256, 8, "test2");
		inp = msgb_alloc_headroom(256, 8, "test1");
		msgb_put(inp, 1);
		inp->l2h = msgb_put(inp, rewrite_results_to_msc[i].inp_len);
		memcpy(inp->l2h, rewrite_results_to_msc[i].input, msgb_l2len(inp));

		rc = bss_patch_filter_msg(inp, &result);
		if (rc < 0) {
			printf("Failed to parse header msg: %d\n", i);
			abort();
		}

		bss_rewrite_header_for_msc(rc, outp, inp, &result);

		memset(&result, 0, sizeof(result));
		rc = bss_patch_filter_msg(outp, &result);
		if (rc < 0) {
			printf("Patched message doesn't work: %d\n", i);
			printf("hex: %s\n", hexdump(outp->l2h, msgb_l2len(outp)));
			abort();
		}

		if (msgb_l2len(outp) != rewrite_results_to_msc[i].exp_len) {
			printf("The length's don#t match on %d %u != %u\n",
				i, msgb_l2len(outp), rewrite_results_to_msc[i].exp_len);
			printf("hex: %s\n", hexdump(outp->l2h, msgb_l2len(outp)));
			abort();
		}

		if (memcmp(outp->l2h, rewrite_results_to_msc[i].expected, rewrite_results_to_msc[i].exp_len) != 0) {
			printf("Expected results don't match for: %d\n", i);
			printf("hex: %s\n", hexdump(outp->l2h, msgb_l2len(outp)));
			abort();
		}

		msgb_free(outp);
		msgb_free(inp);
	}
}

static void test_rewrite_bsc(void)
{
	int i;

	printf("Testing rewriting the SCCP header for BSC.\n");
	for (i = 0; i < sizeof(rewrite_results_to_bsc)/sizeof(rewrite_results_to_bsc[0]); ++i) {
		struct msgb *inp;

		inp = msgb_alloc_headroom(256, 8, "test1");
		msgb_put(inp, 1);
		inp->l2h = msgb_put(inp, rewrite_results_to_bsc[i].inp_len);
		memcpy(inp->l2h, rewrite_results_to_bsc[i].input, msgb_l2len(inp));

		if (bss_rewrite_header_to_bsc(inp, 1, 2) != 0) {
			fprintf(stderr, "Failed to rewrite header\n");
			abort();
		}

		if (memcmp(inp->l2h, rewrite_results_to_bsc[i].expected, msgb_l2len(inp))!= 0) {
			fprintf(stderr, "Content does not match\n");
			printf("got: %s\n", hexdump(inp->l2h, msgb_l2len(inp)));
			abort();
		}

		msgb_free(inp);
	}
}

int main(int argc, char **argv)
{
	test_patch_filter();
	test_rewrite_msc();
	test_rewrite_bsc();
	printf("DONE!\n");
	return 0;
}
