import * as Utils from "../../utils";

const serverError = Utils.Response.error("Internal Server Error.");
const typeError = Utils.Response.error("Data Type Error.");

export { serverError, typeError };
