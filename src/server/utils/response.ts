import * as Interfaces from "../interfaces";

function response<T>(msg: T, status: number): Interfaces.JSON.Response<T> {
  return {
    status,
    msg,
  };
}

function success<T = string>(
  msg: T,
  status = 200
): Interfaces.JSON.Response<T> {
  return response(msg, status);
}

function error(msg: string, status = 500): Interfaces.JSON.Response {
  return response(msg, status);
}

export { success, error };
