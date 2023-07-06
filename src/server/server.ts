import express from "express";
import cors from "cors";
import helmet from "helmet";
import morgan from "morgan";

import * as Middlewares from "./middlewares";
import * as Routers from "./routers";
import * as Constants from "./globals/constants";

const app = express();

// Middlewares
app
  .use(cors())
  .use(helmet())
  .use(morgan("dev"))
  .use(express.json())
  .use(express.urlencoded({ extended: true }));

// Routers
app.use(`${Constants.System.ROOT}/info`, Routers.Info);

// app.get("*", (req, res) => {
//   console.log(req);
//   res.send("Hello World");
// });

// Error Handlers
app.use(Middlewares.Error.errorHandler);

app.listen(5000, () => {
  console.log(`Server running on port ${5000}`);
});
