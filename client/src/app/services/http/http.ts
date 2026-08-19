import { isPlatformBrowser } from '@angular/common';
import { HttpClient } from '@angular/common/http';
import { inject, PLATFORM_ID, Service } from '@angular/core';
import { Observable } from 'rxjs';


@Service()
export class HttpService {
    private readonly m_http = inject(HttpClient)
    private readonly m_platformID = inject(PLATFORM_ID);

    public delete<T>(path: string, withCredentials: boolean=true) : Observable<T> {
        if(isPlatformBrowser(this.m_platformID))
            return this.m_http.delete<T>(path, { withCredentials: withCredentials });

        return new Observable<T>();
    }

    public get<T>(path: string, withCredentials: boolean=true) : Observable<T> {
        if(isPlatformBrowser(this.m_platformID))
            return this.m_http.get<T>(path, { withCredentials: withCredentials });

        return new Observable<T>();
    }

    public patch<T, U=null>(path: string, body: U | null=null, withCredentials: boolean=true) : Observable<T> {
        if(isPlatformBrowser(this.m_platformID))
            return this.m_http.patch<T>(path, body, { withCredentials: withCredentials });

        return new Observable<T>();
    }

    public post<T, U=null>(path: string, body: U | null=null, withCredentials: boolean=true) : Observable<T> {
        if(isPlatformBrowser(this.m_platformID))
            return this.m_http.post<T>(path, body, { withCredentials: withCredentials });

        return new Observable<T>();
    }

    public put<T, U=null>(path: string, body: U | null=null, withCredentials: boolean=true) : Observable<T> {
        if(isPlatformBrowser(this.m_platformID))
            return this.m_http.put<T>(path, body, { withCredentials: withCredentials });

        return new Observable<T>();
    }
}
