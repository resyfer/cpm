import express from "express";

import * as Controllers from "../controllers";

const router = express.Router();

router.get("/:package", Controllers.Info.getPackageInfo);

export default router;
