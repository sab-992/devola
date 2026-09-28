import { ApplicationConfig, inject, PLATFORM_ID, provideAppInitializer, provideBrowserGlobalErrorListeners } from '@angular/core';
import { provideRouter } from '@angular/router';
import { provideHttpClient } from '@angular/common/http';

import { routes } from './app.routes';
import { provideClientHydration } from '@angular/platform-browser';
import { UserService } from '@services/user/user';
import { isPlatformServer } from '@angular/common';
import { of } from 'rxjs';


const authenticate = () => {
    if (isPlatformServer(inject(PLATFORM_ID)))
        return of(null);

    return inject(UserService).authenticate();
}

export const appConfig: ApplicationConfig = {
    providers: [
        provideBrowserGlobalErrorListeners(),
        provideRouter(routes), provideClientHydration(),
        provideHttpClient(),
        provideAppInitializer(authenticate)
    ]
};