import { isPlatformBrowser } from '@angular/common';
import { HttpClient, HttpErrorResponse } from '@angular/common/http';
import { inject, PLATFORM_ID, Service } from '@angular/core';
import { PublisherService } from '@services/publisher/publisher';
import { catchError, EMPTY, Observable, throwError } from 'rxjs';


@Service()
export class HttpService {
    private readonly m_http = inject(HttpClient)
    private readonly m_platformID = inject(PLATFORM_ID);
    private readonly m_publisher: PublisherService = inject(PublisherService);

    public delete<T>(path: string, withCredentials: boolean=true) : Observable<T> {
        if(isPlatformBrowser(this.m_platformID))
            return this.m_http.delete<T>(path, { withCredentials: withCredentials })
                              .pipe(catchError((error: HttpErrorResponse) : Observable<T> => { return this.handleAuthorizationError(error); }));

        return new Observable<T>();
    }

    public get<T>(path: string, withCredentials: boolean=true) : Observable<T> {
        if(isPlatformBrowser(this.m_platformID))
            return this.m_http.get<T>(path, { withCredentials: withCredentials })
                              .pipe(catchError((error: HttpErrorResponse) : Observable<T> => { return this.handleAuthorizationError(error); }));

        return new Observable<T>();
    }

    public patch<T, U=null>(path: string, body: U | null=null, withCredentials: boolean=true) : Observable<T> {
        if(isPlatformBrowser(this.m_platformID))
            return this.m_http.patch<T>(path, body, { withCredentials: withCredentials })
                              .pipe(catchError((error: HttpErrorResponse) : Observable<T> => { return this.handleAuthorizationError(error); }));

        return new Observable<T>();
    }

    public post<T, U=null>(path: string, body: U | null=null, withCredentials: boolean=true) : Observable<T> {
        if(isPlatformBrowser(this.m_platformID))
            return this.m_http.post<T>(path, body, { withCredentials: withCredentials })
                              .pipe(catchError((error: HttpErrorResponse) : Observable<T> => { return this.handleAuthorizationError(error); }));

        return new Observable<T>();
    }

    public put<T, U=null>(path: string, body: U | null=null, withCredentials: boolean=true) : Observable<T> {
        if(isPlatformBrowser(this.m_platformID))
            return this.m_http.put<T>(path, body, { withCredentials: withCredentials })
                              .pipe(catchError((error: HttpErrorResponse) : Observable<T> => { return this.handleAuthorizationError(error); }));

        return new Observable<T>();
    }

    private handleAuthorizationError(error: HttpErrorResponse) {
        if (error.status !== 401)
            return throwError(() => error);

        this.m_publisher.notify("unauthorized", error);
        return EMPTY;
    }
}
