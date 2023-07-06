import { Request, Response, NextFunction } from "express";

type Sync = (
  req: Request,
  res: Response,
  next: NextFunction
) => Response | void;

type Async = (
  req: Request,
  res: Response,
  next: NextFunction
) => Promise<Response | void>;

export type { Sync, Async };
