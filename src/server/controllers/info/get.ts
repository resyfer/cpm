import type * as Interfaces from "../../interfaces";
import * as Utils from "../../utils";
import { prisma } from "../../utils";

const getPackageInfo: Interfaces.Controllers.Async = async (req, res) => {
  const { pkg } = <{ pkg: string }>req.query;

  const info = await prisma.package.findFirst({
    where: {
      name: pkg,
    },
  });

  return res.json(Utils.Response.success(info, info ? 200 : 404));
};

export { getPackageInfo };
