typedef struct {
    asid_t ASID;
    vpn_t VPN; 
    mask_t Mask;
    pfn_t PFN;
    flag_t Flag;
} TLB_page_table_entry_t;

TLB_page_table_entry_t TLB_table[TLB_SIZE];

pfn_t address_translation(asid_t ASID, vpn_t VPN) {
    search_result_t search_result;

    if (!(search_result = TLB_probe(ASID, VPN))) {
        raise_exception(EXCEPTION_TLB_REFILL);
    }
    if (!search_result.V) {
        raise_exception(EXCEPTION_TLB_INVALID);
    }
    if (!search_result.D) {
        raise_exception(EXCEPTION_TLB_MODIFY);
    }
    return search_result.PFN;
}

void handle_TLB_refill(void) {
    context_t context = get_cp0(CP0_CONTEXT);
    pfn_t PFN = get_PFN_from_page_table(context);
    TLB_write(context, PFN);
    return;
}

void handle_TLB_invalid(void) {
    pte_t pte = get_pte_current();
    read_page_from_extern();
}

void handle_TLB_modify(void) {
    pte_t pte = get_pte_current();
    if (check_something()) {
        enable_write();
    } else {
        kill();
    }
}