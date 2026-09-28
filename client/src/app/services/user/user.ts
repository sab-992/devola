import { HttpErrorResponse } from '@angular/common/http';
import { inject, OnDestroy, PLATFORM_ID, Service, signal } from '@angular/core';
import { Router } from '@angular/router';
import { GenericSubscriber } from '@classes/generic-subscriber/subscriber';
import { environment } from '@environments/environment';
import { Login } from '@models/login';
import { Register } from '@models/register';
import { Subscription } from '@models/subscription';
import { HttpRequestFct, HttpService } from '@services/http/http';
import { PublisherService } from '@services/publisher/publisher';
import { EMPTY, Observable, of, } from 'rxjs';
import { routes } from '@config/app.routes';
import { AuthorizationError } from '@models/authorization-error';
import { HttpErrorCallback } from '@models/http-options';
import { isPlatformBrowser } from '@angular/common';


@Service()
export class UserService implements OnDestroy {
    private readonly m_platformID = inject(PLATFORM_ID)
    private readonly m_isAuthenticated = signal(false);
    private readonly m_http: HttpService = inject(HttpService);
    private readonly m_publisher: PublisherService = inject(PublisherService);
    private readonly m_router: Router = inject(Router);
    private readonly m_subscribed: Subscription[] = [];
    private static isHandlingAuthError: boolean = false;

    constructor() {
        this.m_subscribed.push(this.m_publisher.subscribe("unauthorized", GenericSubscriber.create((data: object) => {
            if (!UserService.isHandlingAuthError)
                this.handleAuthenticationError(data as AuthorizationError).subscribe();
        })));
    }

    public ngOnDestroy(): void {
        this.m_subscribed.forEach((subscription) => {
            this.m_publisher.unsubscribe(subscription.event, subscription.subscriber);
        });
    }

    public get isAuthenticated() {
        return this.m_isAuthenticated.asReadonly();
    }

    public authenticate(): Observable<void> {
        if (!isPlatformBrowser(this.m_platformID)) return of(void 0);

        return new Observable<void>(subscriber => {
            this.m_http.post<null>(this.buildPath("/authenticate"), null, {
                next: this.handleAuthenticate.bind(this),
                complete: () => {
                    subscriber.next();
                    subscriber.complete();
                },
            });
        });
    }

    public login(loginInformation: Login, callback: HttpErrorCallback | undefined=undefined) {
        this.m_http.post<null, Login>(this.buildPath("/login"), loginInformation, { next: this.handleLogin.bind(this),
                                                                                    error: callback });
    }

    public logout(callback: HttpErrorCallback | undefined=undefined) {
        if (this.isAuthenticated())
            this.m_http.post<null>(this.buildPath("/auth/logout"), null, { next: this.handleLogout.bind(this),
                                                                           error: callback });
    }

    public register(registerInformation: Register, callback: () => void, errorCallback: HttpErrorCallback | undefined=undefined) {
        if (this.isAuthenticated())
            this.m_http.post<null, Register>(this.buildPath("/register"), registerInformation, { next: callback,
                                                                                                error: errorCallback });
    }

    private buildPath(path: string) {
        return `${environment.apiUrl}/user${path}`;
    }

    private handleAuthenticate() {
        this.m_isAuthenticated.set(true);
    }

    private handleAuthenticationError(authError: AuthorizationError): Observable<void> {
        if (UserService.isHandlingAuthError)
            return EMPTY;

        UserService.isHandlingAuthError = true;

        if (authError.error.status === 401 && this.m_isAuthenticated())
            return new Observable<void>(_ => { this.refresh(authError.handler); });

        UserService.isHandlingAuthError = false;
        this.m_isAuthenticated.set(false);
        this.redirect();
        return EMPTY;
    }

    private handleRefreshError(_: HttpErrorResponse) {
        this.m_isAuthenticated.set(false);
        this.redirect();
    }

    private handleLogin() {
        this.m_isAuthenticated.set(true);
        this.m_router.navigate(['/feed']);
    }

    private handleLogout() {
        this.m_isAuthenticated.set(false);
        this.m_router.navigate(['/']);
    }

    private redirect() {
        this.m_isAuthenticated.set(false);
        const KNOWN_PATHS = routes.map(r => r.path).filter(p => p !== '**' && p !== undefined);
        const currentPath = window.location.pathname.replace(/^\//, '');
        const isKnownRoute = KNOWN_PATHS.includes(currentPath);

        if (isKnownRoute)
            this.m_router.navigate(["/"]);
    }

    private refresh(retryHandler: HttpRequestFct) {
        return this.m_http.post<null>(this.buildPath("/auth/refresh"), null, { next: ()=>{ this.handleAuthenticate.bind(this);
                                                                                           retryHandler(); },
                                                                               error: this.handleRefreshError.bind(this), 
                                                                               complete: () => { UserService.isHandlingAuthError = false; }});
    }
}