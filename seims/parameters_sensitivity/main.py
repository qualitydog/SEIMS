#! /usr/bin/env python
# -*- coding: utf-8 -*-
"""Main entrance for parameters sensitivity analysis
    @author   : Liangjun Zhu
    @changelog: 17-12-23  lj - initial implementation.\n
                18-1-11   lj - code refactor to support multiple psa methods.\n
                18-02-09  lj - compatible with Python3.\n
"""
from __future__ import absolute_import

import time

from parameters_sensitivity.sensitivity import Sensitivity
from parameters_sensitivity.config import get_psa_config, PSAConfig


def main():
    """MAIN FUNCTION."""
    cf, psa_mtd = get_psa_config()

    print('### START TO PARAMETERS SENSITIVITY ANALYSIS ###')
    start_t = time.time()

    cfg = PSAConfig(cf, method=psa_mtd)
    saobj = Sensitivity(cfg)
    saobj.run()

    print('### END OF PARAMETERS SENSITIVITY ANALYSIS ###')
    print('Running time: %.2fs' % (time.time() - start_t))

    # Plot figures
    saobj.plot()


if __name__ == '__main__':
    main()