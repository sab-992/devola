import { HttpErrorResponse } from "@angular/common/http";


export type HttpCallback<T> = (data: T) => void;
export type HttpErrorCallback = (error: HttpErrorResponse) => void;
export type CompleteCallback = () => void;

interface ObservableHandler<T> {
    next?: HttpCallback<T>,
    error?: HttpErrorCallback,
    complete?: CompleteCallback
}

export interface HttpOptions<T> extends ObservableHandler<T> {
    withCredentials?: boolean
}