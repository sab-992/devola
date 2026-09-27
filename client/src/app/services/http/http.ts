import { isPlatformBrowser } from '@angular/common';
import { HttpClient, HttpErrorResponse } from '@angular/common/http';
import { inject, PLATFORM_ID, Service } from '@angular/core';
import { PublisherService } from '@services/publisher/publisher';
import { catchError, EMPTY, throwError } from 'rxjs';
import { HttpOptions } from '@models/http-options';

export type HttpRequestFct = ()=>void;

const DEFAULT_WITH_CREDENTIALS: boolean = true;

@Service()
export class HttpService {
    private readonly m_http = inject(HttpClient)
    private readonly m_platformID = inject(PLATFORM_ID);
    private readonly m_publisher: PublisherService = inject(PublisherService);

    public delete<T>(path: string, options: HttpOptions<T>={}) : void {
        if(isPlatformBrowser(this.m_platformID))
            this.m_http.delete<T>(path, { withCredentials: this.withCredentials<T>(options) })
                       .pipe(catchError((error: HttpErrorResponse)=> {
                            const delete_ = this.delete<T>;
                            return this.handleAuthorizationError<T, undefined>(delete_.bind(this, path, options), error);
                        }))
                        .subscribe(options);
    }

    public get<T>(path: string, options: HttpOptions<T>) : void {
        if(isPlatformBrowser(this.m_platformID))
            this.m_http.get<T>(path, { withCredentials: this.withCredentials<T>(options) })
                       .pipe(catchError((error: HttpErrorResponse)=> {
                            const get = this.get<T>;
                            return this.handleAuthorizationError<T, undefined>(get.bind(this, path, options), error);
                        }))
                        .subscribe(options);
    }

    public patch<T, U=null>(path: string, body: U | null=null, options: HttpOptions<T>) : void {
        if(isPlatformBrowser(this.m_platformID))
            this.m_http.patch<T>(path, body, { withCredentials: this.withCredentials<T>(options) })
                       .pipe(catchError((error: HttpErrorResponse)=> {
                            const patch = this.patch<T, U>;
                            return this.handleAuthorizationError<T, U>(patch.bind(this, path, body, options), error);
                        }))
                        .subscribe(options);
    }

    public post<T, U=null>(path: string, body: U | null=null, options: HttpOptions<T>) : void {
        if(isPlatformBrowser(this.m_platformID))
            this.m_http.post<T>(path, body, { withCredentials: this.withCredentials<T>(options) })
                       .pipe(catchError((error: HttpErrorResponse)=> {
                            const post = this.post<T, U>;
                            return this.handleAuthorizationError<T, U>(post.bind(this, path, body, options), error);
                        }))
                        .subscribe(options);
    }

    public put<T, U=null>(path: string, body: U | null=null, options: HttpOptions<T>) : void {
        if(isPlatformBrowser(this.m_platformID))
            this.m_http.put<T>(path, body, { withCredentials: this.withCredentials<T>(options) })
                       .pipe(catchError((error: HttpErrorResponse)=> {
                            const put = this.put<T, U>;
                            return this.handleAuthorizationError<T, U>(put.bind(this, path, body, options), error);
                        }))
                        .subscribe(options);
    }

    private handleAuthorizationError<T, U=undefined>(failedRequest: HttpRequestFct, error: HttpErrorResponse) {
        if (error.status !== 401)
            return throwError(() => error);

        this.m_publisher.notify("unauthorized", { handler: failedRequest, error});
        return EMPTY;
    }

    private withCredentials<T>(options: HttpOptions<T>) : boolean {
        return options.withCredentials === undefined ? DEFAULT_WITH_CREDENTIALS : options.withCredentials;
    }
}
